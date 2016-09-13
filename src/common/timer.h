/**
* @file  timer.h
* @brief  定时器
* @author  zhaozhongguo
* @date  2016-9-12 
* @version  1.0
*/
#ifndef __TIMER_H
#define __TIMER_H
#include "thread.h"

namespace common
{
    typedef void (*RUNNABLE)();

    /** 
    * @brief 定时器
    */ 
    class CTimer
    {
    public:
        CTimer():m_nDelay(0),m_nPeriod(0),m_bStop(false),m_pfnRunnable(NULL)
        {}

        /** 
        * @brief 执行定时任务
        * @param runnable: 回调函数指针 void (*RUNNABLE)()
        * @param delay: 延时执行时间，单位ms
        * @param period: 执行周期，单位ms
        */
        void schedule(const RUNNABLE runnable, int delay, int period);

        /** 
        * @brief 停止定时器
        */
        void stop()
        {
            m_bStop = true;
        }

        /*
         * @brief 获取延迟时间
         * @return 返回延迟时间，单位ms
         */
        int getDelay()
        {
            return m_iDelay;
        }

        /*
         * @brief 获取周期
         * @return 返回周期，单位ms
         */
        int getPeriod()
        {
            return m_iPeriod;
        }

        /*
         * @brief 获取回调函数
         * @return 返回回调函数指针 void (*RUNNABLE)()
         */
        RUNNABLE getRunnable()
        {
            return m_fnRunnable;
        }

        /** 
        * @brief 判断定时器是否停止
        * @return 停止返回true，未停止返回false
        */
        bool isStop()
        {
            return m_bStop;
        }

    private:
        static void* run(void* param);

    private:
        common::CThread m_iThread;
        int m_nDelay;
        int m_nPeriod;
        RUNNABLE m_pfnRunnable;
        volatile bool m_bStop;
    };

}
#endif