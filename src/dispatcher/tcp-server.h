/**
* @file  tcp-server.h
* @brief  常见类型定义
* @author  zhaozhongguo
* @date  2016-9-12 
* @version  1.0
*/
#ifndef __TCP_SERVER_H
#define __TCP_SERVER_H
#include "type-def.h"
#include "thread.h"

namespace dispatcher
{
    enum ENUM_SOCKET_EVENT
    {
        SOCKET_CONNECT_EVENT,
        SOCKET_READ_EVENT,
        SOCKET_WRITE_EVENT,
        SOCKET_DISCONNECT_EVENT
    }


    /** 
    * @brief tcp服务器 
    * 用于建立tcp监听服务
    */ 
    class CTcpServer
    {
    public:
        CTcpServer(uint32_t ip, uint16_t port, int maxConns)
            :m_nIp(ip),m_nPort(port),m_nMaxConns(maxConns)
        {}

        void addCallback(ENUM_SOCKET_EVENT event, void* (*callback)(void*))
        {

        }

        bool start();
        void stop();

    private:
        uint32_t m_nIp;
        uint16_t m_nPort;
        int m_nMaxConns;
        common::CThread m_iThread;
    };                         
}

#endif