/**
* @file  epoll.h
* @brief  epoll
* @author  zhaozhongguo
* @date  2016-9-13 
* @version  1.0
*/
#ifndef __EPOLL_H
#define __EPOLL_H
#include <sys/epoll.h>
#include <stdlib.h>
#include "type-def.h"
#include "log.h"

namespace common
{
    /** 
    * @brief epoll
    */ 
    class CEpoll
    {
    public:
        CEpoll():m_nEpollFd(-1),m_nMaxEvents(0),m_pEvents(NULL)
        {}

        ~CEpoll()
        {
            if (m_pEvents)
            {
                free(m_pEvents);
            }
        }

        /** 
        * @brief 创建epoll句柄
        * @return 成功返回true，失败返回false
        */
        bool create(int size)
        {
            if ((size <= 0) || (-1 == (m_nEpollFd = epoll_create(size))))
            {
                LOG_ERROR("invalid size or epoll_create failed!");
                return false;
            }

            m_nMaxEvents = size;
            m_pEvents = (struct epoll_event*)malloc(sizeof(struct epoll_event) * m_nMaxEvents);
            return true;
        }

        /** 
        * @brief 控制某个epoll文件描述符上的事件
        * @param op: EPOLL_CTL_ADD 注册、EPOLL_CTL_MOD 修 改、EPOLL_CTL_DEL 删除
        * @param fd: 关联的文件描述符
        * @param flag: 事件类型，可同时设置多个，如：EPOLLIN|EPOLLOUT
        *    EPOLLIN ：表示对应的文件描述符可以读；
        *    EPOLLOUT：表示对应的文件描述符可以写；
        *    EPOLLPRI：表示对应的文件描述符有紧急的数据可读
        *    EPOLLERR：表示对应的文件描述符发生错误；
        *    EPOLLHUP：表示对应的文件描述符被挂断；
        *    EPOLLET：表示对应的文件描述符有事件发生；
        * @return 成功返回true，失败返回false
        */
        bool epollCtl(int op, int fd, uint32_t flag)
        {
            struct epoll_event ev;
            ev.events = flag;
            ev.data.fd = fd;
            if (-1 == epoll_ctl(m_nEpollFd, op, fd, &ev))
            {
                LOG_ERROR("epoll_ctl failed!");
                return false;
            }

            return true;
        }

        /** 
        * @brief 等待I/O事件的发生
        * @return 成功返回发生事件数，失败返回-1
        */
        int epollWait()
        {
            int nfds = -1;
            if (-1 == (nfds = epoll_wait(m_nEpollFd, m_pEvents, m_nMaxEvents, -1)))
            {
                LOG_ERROR("epoll_wait failed!");
                return -1;
            }

            return nfds;
        }

        /** 
        * @brief 获取事件集合
        * @return 返回事件集合
        */
        const struct epoll_event* getEvents()
        {
            return m_pEvents;
        }

    private:
        int m_nEpollFd;
        int m_nMaxEvents;
        struct epoll_event* m_pEvents;
    };
}

#endif