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
*   File Name   : CTCPServer.cpp           *
*   Author      : bnc zgzhao           *
*   Create Date : 2016-7-21           *
*   Version     : 1.0           *
*   Function    : .           *
*                                                                             *
******************************************************************************/
#include "CTCPServer.h"
#include "bnc-error.h"
#include "openflow-10.h"
#include "openflow-13.h"
#include "openflow-common.h"
#include "CBuffer.h"
#include "CRecvWorker.h"
#include "log.h"
#include "comm-util.h"
#include "CServer.h"
#include "COfMsgUtil.h"



static INT4 g_iWorkerIndex = 0;

CTCPServer::CTCPServer(
    UINT4 ip, 
    UINT4 port, 
    INT4 threadNum, 
    INT4 maxOpenFiles,
    CSmartPtr<CRecvWorker> ptrRecvWorker):
    m_uiIP(ip),
    m_uiPort(port),
    m_iListenfd(-1),
    m_iRecvBufLen(RECV_BUF_SIZE),
    m_iMaxEpollFiles(maxOpenFiles),
    m_ptrRecvWorker(ptrRecvWorker),
    m_iRecvThreadSize(threadNum)
{
}


CTCPServer::~CTCPServer()
{
    m_mapRecvBuf.clear();
}

void* startServer(void* param)
{
    if (NULL == param)
    {
        LOG_ERROR_FMT("startServer failure");
        return NULL;
    }

    CTCPServer* pServer = (CTCPServer*)param;
    LOG_INFO_FMT("CTCPServer thread start, id:%llu", (UINT8)pServer->m_threadId);

    //监听端口
    if(FALSE == pServer->CTCPServer::listen())
    {
        LOG_ERROR("listen local port failure");
        return NULL;
    }

    //初始化worker
    INT4 max_num = pServer->m_iMaxEpollFiles/pServer->m_iRecvThreadSize + 10;
    for (INT4 i = 0; i < pServer->m_iRecvThreadSize; ++i)
    {
        if (FALSE == pServer->m_workerVec[i]->init(max_num, pServer->m_iRecvBufLen, &(pServer->m_mapRecvBuf)))
        {
            LOG_ERROR("init recv worker failure");
            close(pServer->m_iListenfd);
            return NULL;
        }
    }

    pServer->process();

    close(pServer->m_iListenfd);

    return NULL;
}

BOOL CTCPServer::init()
{
    const INT1* bufferSize = CServer::getInstance()->getConf()->getConfig("controller", "recv_buffer_size");
    m_iRecvBufLen = (bufferSize == NULL) ? RECV_BUF_SIZE : atol(bufferSize);
    
    //初始化worker列表
    for (INT4 i = 0; i < m_iRecvThreadSize; ++i)
    {
        m_workerVec.push_back(CSmartPtr<CRecvWorker>(m_ptrRecvWorker->clone()));
    }

    pthread_create(&m_threadId, NULL, startServer, (void*)this);
    LOG_INFO("init CTCPServer success");
    return TRUE;
}

BOOL CTCPServer::listen()
{
   //创建监听socket
    INT4 sockfd = socket(AF_INET , SOCK_STREAM , 0);
    if(sockfd < 0 )
    {
        LOG_ERROR("create listen socket failure");
        return FALSE;
    }

    //设置地址可重用
    INT4 sockopt = 1;
    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, (INT1*)&sockopt, sizeof(sockopt));

    //设置监听fd为非阻塞模式
    setNonBlocking(sockfd);

    struct sockaddr_in address;
    memset(&address , 0 , sizeof(struct sockaddr_in));
    address.sin_family = AF_INET;
    address.sin_port = htons(m_uiPort);
    address.sin_addr.s_addr = 0;
    //绑定监听地址
    if(bind(sockfd, (struct sockaddr *)&address, sizeof(address)) < 0)
    {
        LOG_ERROR("bind failure");
        close(sockfd);
        return FALSE;
    }

    //监听端口
    if (::listen(sockfd, m_iMaxEpollFiles) < 0)
    {
        LOG_ERROR("listen failure");
        close(sockfd);
        return FALSE;
    }

    m_iListenfd = sockfd;

    LOG_INFO_FMT("listen port:%u success", m_uiPort);
    return TRUE;
}


void CTCPServer::process()
{
    //创建epollfd
    epoll_event events[1];
    INT4 epollfd = epoll_create(1);
    if (epollfd < 0)
    {
        LOG_ERROR("epoll_create failure");
        close(m_iListenfd);
        return;
    }
    
    if (FALSE == addFd(epollfd, m_iListenfd))
    {
        LOG_ERROR("add listenfd failure");
        close(m_iListenfd);
        return;
    }

    INT4 ret_wait = -1;
    INT4 sockfd = -1;
    while (TRUE)
    {
        try
        {
            if ((ret_wait = epoll_wait(epollfd, events, 1, -1)) < 0)
            {
                LOG_ERROR("epoll_wait failure");
                continue;
            }

            for (INT4 i = 0; i < ret_wait; i++)
            {
                sockfd = events[i].data.fd;
                if (sockfd == m_iListenfd)
                {
                    accept();
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
}


void CTCPServer::accept()
{
    INT4 connfd = -1;
    struct sockaddr_in client_address;
    socklen_t client_addrlength = sizeof(client_address);
    while ((connfd = ::accept(m_iListenfd, (struct sockaddr*)&client_address, &client_addrlength)) > 0)
    {
        UINT2 port = ntohs(client_address.sin_port);
        LOG_INFO_FMT("new connection ip:%s, port:%d", 
            inet_ntoa(client_address.sin_addr),
            port);
        //分配接受缓冲区
        if (m_mapRecvBuf.end() == m_mapRecvBuf.find(connfd))
        {
            m_mapRecvBuf.insert(std::pair<INT4, CSmartPtr<CBuffer> >(connfd, CSmartPtr<CBuffer>(new CBuffer(m_iRecvBufLen))));
        }

        //分配发送缓冲区
        if (COfMsgUtil::g_mapSendBuf.end() == COfMsgUtil::g_mapSendBuf.find(connfd))
        {
            COfMsgUtil::g_mapSendBuf.insert(std::pair<INT4, CSmartPtr<CBuffer> >(connfd, CSmartPtr<CBuffer>(new CBuffer(m_iRecvBufLen))));
        }

        UINT4 clientIp = ntohl(client_address.sin_addr.s_addr);

        if (FALSE == m_workerVec[g_iWorkerIndex++ % m_iRecvThreadSize]->addConnectFd(connfd, clientIp, port))
        {
            LOG_ERROR("accept failure");
        }
    }
}