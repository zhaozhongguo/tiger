/**
* @file  cond.h
* @brief  条件变量定义
* @author  zhaozhongguo
* @date  2016-9-12 
* @version  1.0
*/
#ifndef __COND_H
#define __COND_H
#include <pthread.h>
#include "mutex.h"

namespace common
{
    /** 
    * @brief 条件变量
    */ 
    class CCond
    {
    public:
        CCond() {};

        /** 
        * @brief 创建条件变量
        * @return 成功返回true，失败返回false
        */
        bool create()
        {
            int err = pthread_cond_init(&m_iCond, NULL);
            if (0 != err) 
            {
                LOG_ERROR_FMT("pthread_cond_init() failed! errcode=%d", err);
                return false;
            }

            return true;
        }

        /** 
        * @brief 销毁条件变量
        * @return 成功返回true，失败返回false
        */
        bool destroy()
        {
            int err = pthread_cond_destroy(&m_iCond);
            if (0 != err) 
            {
                LOG_ERROR_FMT("pthread_cond_destroy() failed! errcode=%d", err);
                return false;
            }

            return true;
        }

        /** 
        * @brief 释放一个阻塞在该条件变量上的一个线程
        * usage: lock signal unlock
        * @return 成功返回true，失败返回false
        */
        bool signal()
        {
            int err = pthread_cond_signal(&m_iCond);
            if (0 != err) 
            {
                LOG_ERROR_FMT("pthread_cond_signal() failed! errcode=%d", err);
                return false;
            }

            return true;
        }

        /** 
        * @brief 使线程阻塞在该条件变量
        * usage: lock wait unlock
        * @return 成功返回true，失败返回false
        */
        bool wait(const common::CMutex& mutex)
        {
            int err = pthread_cond_wait(&m_iCond, &mutex.m_iMutex);
            if (0 != err) 
            {
                LOG_ERROR_FMT("pthread_cond_wait() failed! errcode=%d", err);
                return false;
            }

            return true;
        }

    private:
        pthread_cond_t m_iCond;
    };

}
#endif