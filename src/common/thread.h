/**
* @file  thread.h
* @brief  线程定义
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
    /** 
    * @brief 线程
    */ 
    class CThread
    {

    public:
        CThread() {};

        /** 
        * @brief 创建线程
        * @param attr: 线程属性
        * @param run: 线程入口函数
        * @param arg: 入口函数的参数
        * @return 成功返回true，失败返回false
        */
        bool create(const pthread_attr_t* attr, void* (*run)(void*), void* arg)
        {
            int err = pthread_create(&m_nThreadId, attr, run, arg);
            if (0 != err)
            {
                LOG_ERROR_FMT("pthread_create() failed! errcode=%d", err);
                return false;
            }

            return true;
        }

        /** 
        * @brief 获取线程id
        * @return 返回线程id
        */
        pthread_t getThreadId()
        {
            return m_nThreadId;
        }

    private:
        pthread_t m_nThreadId;
    };
}

#endif