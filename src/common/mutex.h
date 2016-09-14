/**
* @file  mutex.h
* @brief  互斥锁
* @author  zhaozhongguo
* @date  2016-9-12 
* @version  1.0
*/
#ifndef __MUTEX_H
#define __MUTEX_H
#include <pthread.h>
#include "log.h"

namespace common
{
    class CCond;

    /** 
    * @brief 互斥锁
    */ 
    class CMutex
    {
    friend class CCond;

    public:
        CMutex() {};

        /** 
        * @brief 创建锁
        * @return 成功返回true，失败返回false
        */
        bool create();

        /** 
        * @brief 销毁锁
        * @return 成功返回true，失败返回false
        */
        bool destroy()
        {
            int err = pthread_mutex_destroy(&m_iMutex);
            if (0 != err) 
            {
                LOG_ERROR_FMT("pthread_mutex_destroy() failed! errcode=%d", err);
                return false;
            }

            return true;
        }

        /** 
        * @brief 加锁
        * @return 成功返回true，失败返回false
        */
        bool lock()
        {
            int err = pthread_mutex_lock(&m_iMutex);
            if (0 != err) 
            {
                LOG_ERROR_FMT("pthread_mutex_lock() failed! errcode=%d", err);
                return false;
            }

            return true;
        }

        /** 
        * @brief 解锁
        * @return 成功返回true，失败返回false
        */
        bool unlock()
        {
            int err = pthread_mutex_unlock(&m_iMutex);
            if (0 != err) 
            {
                LOG_ERROR_FMT("pthread_mutex_unlock() failed! errcode=%d", err);
                return false;
            }

            return true;
        }

    private:
        pthread_mutex_t m_iMutex;
    };
}

#endif