/**
* @file  timer.cpp
* @brief  定时器
* @author  zhaozhongguo
* @date  2016-9-12 
* @version  1.0
*/
#include "timer.h"
#include "log.h"

namespace common
{
    void CTimer::schedule(const RUNNABLE runnable, int delay, int period)
    {
        m_bStop = false;
        m_nDelay = delay;
        m_nPeriod = period;
        m_pfnRunnable = runnable;
        m_iThread.create(NULL, run, (void*)this);
    }


    void* CTimer::run(void* param)
    {
        CTimer* timer = (CTimer*)param;
        if (timer->isStop())
        {
            return NULL;
        }

        // int cpuId = (CServer::getInstance()->getCpuId()++) % getTotalCpu();
        // setCpuAffinity(cpuId);

        int delay = timer->getDelay() * 1000;
        int period = timer->getPeriod() * 1000;
        RUNNABLE runnable = timer->getRunnable();
        usleep(delay);
        while (TRUE)
        {
            if (timer->isStop())
            {
                return NULL;
            }

            try
            {
                runnable();
            }
            catch (...)
            {
                LOG_ERROR("catch exception !");
            }

            usleep(period);
        }
        return NULL;
    }
}