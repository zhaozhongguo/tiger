/**
* @file  tcp-server.cpp
* @brief  tcp服务
* @author  zhaozhongguo
* @date  2016-9-13
* @version  1.0
*/
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include "tcp-server.h"
#include "log.h"
#include "tools.h"

namespace dispatcher
{
    void* CTcpServer::run(void* param)
    {
        if (NULL == param)
        {
            LOG_ERROR("CTcpServer thread, invalid param!");
            return NULL;
        }

        // int cpuId = (CServer::getInstance()->getCpuId()++) % getTotalCpu();
        // setCpuAffinity(cpuId);

        CTcpServer* pServer = (CTcpServer*)param;
        if (!pServer->listen())
        {
            return NULL;
        }

        pServer->process();

        return NULL;
    }


    bool CTcpServer::listen()
    {
       //创建监听socket
        int m_nListenFd = socket(AF_INET , SOCK_STREAM , 0);
        if(m_nListenFd < 0)
        {
            LOG_ERROR("create listen socket failure");
            return false;
        }

        //设置地址可重用
        int sockopt = 1;
        setsockopt(m_nListenFd, SOL_SOCKET, SO_REUSEADDR, (char*)&sockopt, sizeof(sockopt));

        //设置监听fd为非阻塞模式
        common::setNonBlocking(m_nListenFd);

        struct sockaddr_in address;
        memset(&address , 0 , sizeof(struct sockaddr_in));
        address.sin_family = AF_INET;
        address.sin_port = htons(m_nPort);
        address.sin_addr.s_addr = 0;
        //绑定监听地址
        if(bind(m_nListenFd, (struct sockaddr *)&address, sizeof(address)) < 0)
        {
            LOG_ERROR("bind failure");
            close(m_nListenFd);
            return false;
        }

        //监听端口
        if (::listen(m_nListenFd, m_nMaxConns) < 0)
        {
            LOG_ERROR("listen failure");
            close(m_nListenFd);
            return false;
        }

        LOG_INFO_FMT("listen port:%u success", m_nPort);
        return true;
    }



    void CTcpServer::process()
    {
        //创建epollfd
        if (!m_iEpoll.create(m_nMaxConns))
        {
            return;
        }

        //ET模式监听读事件
        if (m_iEpoll.epollCtl(EPOLL_CTL_ADD, m_nListenFd, EPOLLIN | EPOLLET))
        {
            return;
        }

        int ret = -1;
        int fd = -1;
        const struct epoll_event* events = m_iEpoll.getEvents();
        while (true)
        {
            try
            {
                if (m_bStop)
                {
                    return;
                }

                if ((ret = m_iEpoll.epollWait()) < 0)
                {
                    continue;
                }

                for (int i = 0; i < ret; i++)
                {
                    fd = events[i].data.fd;

                    //new connection event
                    if (fd == m_nListenFd)
                    {
                        acceptEvent();
                    }
                    //read event
                    if (events[i].events & EPOLLIN)
                    {
                        readEvent(fd);
                    }
                    //write event
                    else if (events[i].events & EPOLLOUT)
                    {
                        writeEvent(fd);
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


    void CTcpServer::acceptEvent()
    {
        //未注册事件不处理
        std::map<ENUM_SOCKET_EVENT, CallBack>::iterator iter = m_mRegisters.find(SOCKET_CONNECT_EVENT);
        if (iter == m_mRegisters.end())
        {
            return;
        }

        int connfd = -1;
        struct sockaddr_in client_address;
        socklen_t client_addrlength = sizeof(client_address);
        while ((connfd = ::accept(m_nListenFd, (struct sockaddr*)&client_address, &client_addrlength)) > 0)
        {
            uint16_t port = ntohs(client_address.sin_port);
            uint32_t clientIp = ntohl(client_address.sin_addr.s_addr);
            if (m_iEpoll.epollCtl(EPOLL_CTL_ADD, connfd, EPOLLIN | EPOLLET))
            {
                continue;
            }

            //connfd, clientIp, port
            Connection_t connect;
            connect.sockfd = connfd;
            connect.ip = clientIp;
            connect.port = port;
            CallBack callback = iter->second;
            callback((void*)&connect);
        }
    }


    void CTcpServer::readEvent(int fd)
    {
        //未注册事件不处理
        std::map<ENUM_SOCKET_EVENT, CallBack>::iterator iter = m_mRegisters.find(SOCKET_READ_EVENT);
        if (iter == m_mRegisters.end())
        {
            return;
        }

        CallBack callback = iter->second;
        callback((void*)&fd);
    }


    void CTcpServer::writeEvent(int fd)
    {
        //未注册事件不处理
        std::map<ENUM_SOCKET_EVENT, CallBack>::iterator iter = m_mRegisters.find(SOCKET_WRITE_EVENT);
        if (iter == m_mRegisters.end())
        {
            return;
        }

        CallBack callback = iter->second;
        callback((void*)&fd);
    }
}