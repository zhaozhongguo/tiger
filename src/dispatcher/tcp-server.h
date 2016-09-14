/**
* @file  tcp-server.h
* @brief  tcp服务
* @author  zhaozhongguo
* @date  2016-9-13 
* @version  1.0
*/
#ifndef __TCP_SERVER_H
#define __TCP_SERVER_H
#include <map>
#include "type-def.h"
#include "thread.h"
#include "epoll.h"

namespace dispatcher
{
    typedef void* (*CallBack)(void*);

    /** 
    * @brief tcp服务器 
    * 用于建立tcp监听服务
    */ 
    class CTcpServer
    {
    public:
        CTcpServer(uint32_t ip, uint16_t port, int maxConns)
            :m_nIp(ip),m_nPort(port),m_nMaxConns(maxConns),m_bStop(false),m_nListenFd(-1)
        {}

        /*
         * @brief 注册事件
         * @param event: 事件类型 @seeENUM_SOCKET_EVENT
         * @param pCallBack: 回调函数，事件触发时回调
         */
        void registerEvent(ENUM_SOCKET_EVENT event, CallBack pCallBack)
        {
            m_mRegisters.insert(std::pair<ENUM_SOCKET_EVENT, CallBack>(event, pCallBack));
        }

        /*
         * @brief 去注册事件
         * @param event: 事件类型 @seeENUM_SOCKET_EVENT
         */
        void unregisterEvent(ENUM_SOCKET_EVENT event)
        {
            std::map<ENUM_SOCKET_EVENT, CallBack>::iterator iter = m_mRegisters.find(event);
            if (iter != m_mRegisters.end())
            {
                m_mRegisters.erase(iter);
            }
        }

        /** 
        * @brief 启动tcp服务
        */
        bool start()
        {
            m_bStop = false;
            return m_iThread.create(NULL, run, (void*)this);
        }

        /** 
        * @brief 停止tcp服务
        */
        void stop()
        {
            m_bStop = true;
        }

        /*
         * @brief 获取epoll
         * @return 返回epoll
         */
        const common::CEpoll& getEpoll()
        {
            return m_iEpoll;
        }

    private:
        //线程入口函数
        static void* run(void* param);

        //监听
        bool listen();

        //循环处理事件
        void process();

        //处理新连接事件
        void acceptEvent();

        //处理读事件
        void readEvent(int fd);

        //处理写事件
        void writeEvent(int fd);

    private:
        uint32_t m_nIp;
        uint16_t m_nPort;
        int m_nMaxConns;
        volatile bool m_bStop;
        int m_nListenFd;

        common::CEpoll m_iEpoll; 
        common::CThread m_iThread;

        std::map<ENUM_SOCKET_EVENT, CallBack> m_mRegisters;
        
    };                         
}

#endif