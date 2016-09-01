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
*   File Name   : COfpRecvWorker.cpp           *
*   Author      : bnc zgzhao           *
*   Create Date : 2016-7-21           *
*   Version     : 1.0           *
*   Function    : .           *
*                                                                             *
******************************************************************************/
#include "COfpRecvWorker.h"
#include "CBuffer.h"
#include "log.h"
#include "CServer.h"
#include "openflow-common.h"




COfpRecvWorker::COfpRecvWorker()
{
}

COfpRecvWorker::~COfpRecvWorker()
{
}

void COfpRecvWorker::processMsgIn(INT4 sockfd, CSmartPtr<CBuffer> inMsg)
{
    CSmartPtr<CMsg> msgPtr(NULL);
    while (!(msgPtr = inMsg->getMsg()).isNull())
    {
        msgPtr->setSockfd(sockfd);
        //消息入队列
        std::string& retPath = CServer::getInstance()->getMsgTreeNodeMgr()->getRootNode()->putMsgToChildNodes(msgPtr);
        msgPtr = CSmartPtr<CMsg>(NULL);
        //通知handler处理新请求
        CServer::getInstance()->getHandlerMgr()->notify(retPath);
    }
}

CRecvWorker* COfpRecvWorker::clone()
{
    return new COfpRecvWorker();
}

void COfpRecvWorker::processConnClosed(INT4 sockfd)
{
    //删除拓扑信息
    const CSmartPtr<CSwitch>& sw = CServer::getInstance()->getSwitchMgr()->findSwBySockFd(sockfd);
    if (!sw.isNull())
    {
        CServer::getInstance()->getTopoMgr()->deleteAllLine(sw->getDpid());
    }
    
    //删除交换机
    CServer::getInstance()->getSwitchMgr()->delSwBySockFd(sockfd);
    LOG_INFO("del switch");
    CServer::getInstance()->getTopoMgr()->updatePaths();
}

BOOL COfpRecvWorker::addConnectFd(INT4 fd, UINT4 ip, UINT2 port)
{
    if (CRecvWorker::addConnectFd(fd, ip, port))
    {
        //新建交换机
        CSmartPtr<CSwitch> sw(new CSwitch());
        sw->setSockFd(fd);
        sw->setSwIp(ip);
        sw->setSwPort(port);
        CServer::getInstance()->getSwitchMgr()->addSw(sw);

        LOG_INFO("add switch");
        return TRUE;
    }

    return FALSE;
}