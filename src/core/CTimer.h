/*
 * GNFlush SDN Controller GPL Source Code
 * Copyright (C) 2016, Greenet <greenet@greenet.net.cn>
 *
 * This file is part of the GNFlush SDN Controller. GNFlush SDN
 * Controller is a free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, , see <http://www.gnu.org/licenses/>.
 */

/******************************************************************************
*                                                                             *
*   File Name   : CTimer.h           *
*   Author      : bnc zgzhao           *
*   Create Date : 2016-5-25           *
*   Version     : 1.0           *
*   Function    : .           *
*                                                                             *
******************************************************************************/
#ifndef __CTIMER_H
#define __CTIMER_H
#include "bnc-type.h"
#include "CRefObj.h"

typedef void (*RUNNABLE)();

/*
 * 定时器类
 */
class CTimer: public CRefObj
{
public:
    CTimer();
    ~CTimer();

    /*
     * 执行定时任务
     * param runnable: 回调函数指针 void (*RUNNABLE)();
     * param delay: 延时执行时间，单位ms
     * param period: 执行周期，单位ms
     */
    void schedule(RUNNABLE runnable, INT4 delay, INT4 period);

    /*
     * 停止定时器
     */
    void stop();

    /*
     * 获取延迟时间
     * ret: 返回延迟时间，单位ms
     */
    INT4 getDelay();

    /*
     * 获取执行周期
     * ret: 返回执行周期，单位ms
     */
    INT4 getPeriod();

    /*
     * 获取回调函数指针
     * ret: 返回回调函数指针
     */
    RUNNABLE getRunnable();

    /*
     * 判断定时器是否已停止
     * ret: 停止 TRUE，未停止 FALSE
     */
    BOOL isStop();
private:
    static void* run(void* param);

private:
    pthread_t m_threadId;
    INT4 m_iDelay;
    INT4 m_iPeriod;
    RUNNABLE m_fnRunnable;
    volatile BOOL m_bStop;
};

#endif