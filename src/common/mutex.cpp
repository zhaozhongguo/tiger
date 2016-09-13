/**
* @file  mutex.cpp
* @brief  互斥锁
* @author  zhaozhongguo
* @date  2016-9-12 
* @version  1.0
*/
#include "mutex.h"
#include "log.h"

namespace common
{
    bool CMutex::create()
    {
        pthread_mutexattr_t attr;
        int err = pthread_mutexattr_init(&attr);
        if (0 != err)
        {
            LOG_ERROR_FMT("pthread_mutexattr_init() failed! errcode=%d", err);
            return false;
        }

        err = pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_ERRORCHECK_NP);
        if (0 != err)
        {
            LOG_ERROR_FMT("pthread_mutexattr_settype() failed! errcode=%d", err);
            return false;
        }

        err = pthread_mutex_init(&m_iMutex, &attr);
        if (0 != err)
        {
            LOG_ERROR_FMT("pthread_mutex_init() failed! errcode=%d", err);
            return false;
        }

        err = pthread_mutexattr_destroy(&attr);
        if (0 != err) 
        {
            LOG_ERROR_FMT("pthread_mutexattr_destroy() failed! errcode=%d", err);
        }

        return true;
    }
}