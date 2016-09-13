/**
* @file  tcp-server.cpp
* @brief  常见类型定义
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
#include "tcp-server.h"
#include "log.h"

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
        while (true)
        {
            if (pServer->isStop())
            {
                return NULL;
            }



        }


    }

    bool CTcpServer::listen()
    {
       //创建监听socket
        int sockfd = socket(AF_INET , SOCK_STREAM , 0);
        if(sockfd < 0)
        {
            LOG_ERROR("create listen socket failure");
            return FALSE;
        }

        //设置地址可重用
        int sockopt = 1;
        setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, (INT1*)&sockopt, sizeof(sockopt));

        //设置监听fd为非阻塞模式
        setNonBlocking(sockfd);

        struct sockaddr_in address;
        memset(&address , 0 , sizeof(struct sockaddr_in));
        address.sin_family = AF_INET;
        address.sin_port = htons(m_nPort);
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

        m_nListenFd = sockfd;

        LOG_INFO_FMT("listen port:%u success", m_uiPort);
        return TRUE;
    }
}