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
*   File Name   : CRecvWorker.cpp           *
*   Author      : bnc zgzhao           *
*   Create Date : 2016-5-25           *
*   Version     : 1.0           *
*   Function    : .           *
*                                                                             *
******************************************************************************/
#include "CRecvWorker.h"
#include "error.h"
#include "CBuffer.h"
#include "log.h"
#include "comm-util.h"
#include "CMsg.h"
#include "CServer.h"
#include "COfMsgUtil.h"




CRecvWorker::CRecvWorker():
    m_iMaxEpollFiles(2000),
    m_threadId(0),
    m_mapRecvBuf(NULL),
    m_strRecvBuf(NULL),
    m_iRecvBufLen(RECV_BUF_SIZE),
    m_epollFd(0)
{
}

CRecvWorker::~CRecvWorker()
{
    if (m_strRecvBuf)
    {
        delete[] m_strRecvBuf;
    }
}

BOOL CRecvWorker::init(
    INT4 maxEpollFiles,
    INT4 recvBufLen,
    std::map<INT4, CSmartPtr<CBuffer> >* recvBuf)
{
    m_iMaxEpollFiles = maxEpollFiles;
    m_iRecvBufLen = recvBufLen;
    m_mapRecvBuf = recvBuf;
    m_strRecvBuf = new UINT1[m_iRecvBufLen];

    m_epollFd = epoll_create(m_iMaxEpollFiles);
    if (m_epollFd < 0)
    {
        LOG_ERROR("epoll_create failure");
        return FALSE;
    }
    pthread_create(&m_threadId, NULL, work, (void*)this);
    LOG_INFO("init CRecvWorker success");
    return TRUE;
}


BOOL CRecvWorker::addConnectFd(INT4 fd, UINT4 ip, UINT2 port)
{
    LOG_INFO_FMT("add fd = %d to worker:%llu ...", fd, (UINT8)m_threadId);
    return addFd(m_epollFd, fd);
}

INT4 CRecvWorker::getEpollFd()
{
    return m_epollFd;
}

INT4 CRecvWorker::getMaxEpollFiles()
{
    return m_iMaxEpollFiles;
}

void* CRecvWorker::work(void* param)
{
    CRecvWorker* worker = (CRecvWorker*)param;
    if (NULL == worker)
    {
        return NULL;
    }
    LOG_INFO_FMT("msg recv worker thread start, id:%llu", (UINT8)worker->m_threadId);
    INT4 cpuId = (CServer::getInstance()->getCpuId()++) % getTotalCpu();
    setCpuAffinity(cpuId);

    epoll_event* events = new epoll_event[worker->getMaxEpollFiles()];
    INT4 ret_wait = -1;
    INT4 sockfd = -1;
    while (TRUE)
    {
        try
        {
            if ((ret_wait = epoll_wait(worker->getEpollFd(), events, 1, -1)) < 0)
            {
                LOG_ERROR("epoll_wait failure");
                continue;
            }

            for (INT4 i = 0; i < ret_wait; i++)
            {
                sockfd = events[i].data.fd; 
                if (events[i].events & EPOLLIN)
                {
                    worker->msgIn(sockfd);
                }
                else if (events[i].events & EPOLLOUT)
                {
                    //TODO
                }
                else
                {

                }
            }
        }
        catch (...)
        {
            LOG_ERROR("catch exception !");
        }
    }

    delete[] events;
}


void CRecvWorker::msgIn(INT4 sockfd)
{
    INT4 ret_recv = -1;
    CSmartPtr<CMsg> msgPtr(NULL);
    std::map<INT4, CSmartPtr<CBuffer> >::iterator iter;
    while (TRUE)
    {
        try
        {
            memset(m_strRecvBuf, 0, m_iRecvBufLen);
            ret_recv = recv(sockfd, m_strRecvBuf, m_iRecvBufLen, 0);
            if (ret_recv < 0)
            {
                //消息读取完毕
                if (errno == EAGAIN || errno == EWOULDBLOCK)
                {
                    iter = m_mapRecvBuf->find(sockfd);
                    if (m_mapRecvBuf->end() != iter)
                    {
                        processMsgIn(sockfd, iter->second);
                    }
                    break;
                }
            }
            //连接断开
            else if (0 == ret_recv)
            {
                close(sockfd);
                //释放接受缓冲区
                iter = m_mapRecvBuf->find(sockfd);
                if (m_mapRecvBuf->end() != iter)
                {
                    m_mapRecvBuf->erase(iter);
                }

                //释放发送缓冲区
                iter = COfMsgUtil::g_mapSendBuf.find(sockfd);
                if (COfMsgUtil::g_mapSendBuf.end() != iter)
                {
                    COfMsgUtil::g_mapSendBuf.erase(iter);
                }

                processConnClosed(sockfd);
                break;
            }
            else
            {
                //读取缓冲区数据到本地接受Cbuffer缓冲区
                iter = m_mapRecvBuf->find(sockfd);
                if (m_mapRecvBuf->end() != iter)
                {
                    iter->second->write(m_strRecvBuf, ret_recv);
                }
            }
        }
        catch (...)
        {
            LOG_ERROR("catch exception !");
        }
    }
}

CRecvWorker* CRecvWorker::clone()
{
    return new CRecvWorker();
}


void CRecvWorker::processMsgIn(INT4 sockfd, CSmartPtr<CBuffer> inMsg)
{
    LOG_INFO("ready to handler msg");
}

void CRecvWorker::processConnClosed(INT4 sockfd)
{
    LOG_INFO("connection closed");
}