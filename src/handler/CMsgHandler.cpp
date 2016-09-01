/*
 * GNFlush SDN Controller GPL Source Code
 * Copyright (C) 2016, Greenet <greenet@greenet.net.cn>
 *
 * This file is part of the GNFlush SDN Controller. GNFlush SDN
 * Controller is a free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, , see <http://www.gnu.org/licenses/>.
 */

/******************************************************************************
*                                                                             *
*   File Name   : CMsgHandler.cpp           *
*   Author      : bnc zgzhao           *
*   Create Date : 2016-5-25           *
*   Version     : 1.0           *
*   Function    : .           *
*                                                                             *
******************************************************************************/
#include "CMsgHandler.h"
#include "comm-util.h"
#include "log.h"
#include "bnc-param.h"
#include "CServer.h"

#define HANDLER_STOP_TIMEOUT 1



CMsgHandler::CMsgHandler():
    m_threadId(0),
    m_bStop(FALSE)
{
}


CMsgHandler::~CMsgHandler()
{
}


void* handlerLoop(void* param)
{
    if (NULL == param)
    {
        LOG_ERROR_FMT("handler thread start failure");
        return NULL;
    }

    CMsgHandler* pHandler = (CMsgHandler*)param;
    LOG_DEBUG_FMT("%s thread start, threadId:%llu", pHandler->toString().c_str(), (UINT8)pHandler->m_threadId);
    INT4 cpuId = (CServer::getInstance()->getCpuId()++) % getTotalCpu();
    setCpuAffinity(cpuId);

    //获取msgTreeNode节点
    CSmartPtr<CMsgTreeNode> msgTreeNode = CServer::getInstance()->getMsgTreeNodeMgr()->getMsgTreeNodeByPath(pHandler->m_strPath);
    if (msgTreeNode.isNull())
    {
        LOG_ERROR_FMT("can not find msg tree node by path : %s, thread exit", pHandler->m_strPath.c_str());
        return NULL;
    }
    
    struct timespec timeout;
    while (TRUE)
    {
        try
        {
            //是否停止线程
            if (pHandler->m_bStop)
            {
                break;
            }

            memset(&timeout, 0, sizeof(timeout));
            timeout.tv_sec = time(0) + HANDLER_STOP_TIMEOUT;
            timeout.tv_nsec = 0;

            //等待消息
            pHandler->m_oMutex.lock();
            pHandler->m_oCond.timedwait(&pHandler->m_oMutex, &timeout);
            pHandler->m_oMutex.unlock();

            //处理消息
            CSmartPtr<CMsgQueue> queue = msgTreeNode->getMsgQueue();
            if (queue.isNull())
            {
                continue;
            }

            pHandler->handle(queue);
            queue->setBusy(FALSE);
        }
        catch (...)
        {
            LOG_ERROR("catch exception !");
        }
    }

    LOG_INFO_FMT("%s thread stop, id:%llu", pHandler->toString().c_str(), (UINT8)pHandler->m_threadId);
    return NULL;
}


void CMsgHandler::start()
{
    pthread_create(&m_threadId, NULL, handlerLoop, (void*)this);
    m_bStop = FALSE;
}


void CMsgHandler::stop()
{
    m_bStop = TRUE;
}


void CMsgHandler::notify()
{
    m_oMutex.lock();
    m_oCond.signal();
    m_oMutex.unlock();
}


void CMsgHandler::setPath(std::string& path)
{
    m_strPath = path;
}

std::string& CMsgHandler::getPath()
{
    return m_strPath;
}