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
*   File Name   : CTimer.cpp           *
*   Author      : bnc zgzhao           *
*   Create Date : 2016-5-25           *
*   Version     : 1.0           *
*   Function    : .           *
*                                                                             *
******************************************************************************/
#include "CTimer.h"
#include "log.h"
#include "comm-util.h"
#include "CServer.h"



CTimer::CTimer():m_threadId(0),m_iDelay(0),m_iPeriod(0),m_bStop(FALSE),m_fnRunnable(NULL)
{
}

CTimer::~CTimer()
{
}

void CTimer::schedule(RUNNABLE runnable, INT4 delay, INT4 period)
{
    m_bStop = FALSE;
    m_iDelay = delay;
    m_iPeriod = period;
    m_fnRunnable = runnable;
    pthread_create(&m_threadId, NULL, run, (void*)this);
}


void* CTimer::run(void* param)
{
    CTimer* timer = (CTimer*)param;
    if (timer->isStop())
    {
        return NULL;
    }

    INT4 cpuId = (CServer::getInstance()->getCpuId()++) % getTotalCpu();
    setCpuAffinity(cpuId);

    INT4 delay = timer->getDelay() * 1000;
    INT4 period = timer->getPeriod() * 1000;
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

INT4 CTimer::getDelay()
{
    return m_iDelay;
}

INT4 CTimer::getPeriod()
{
    return m_iPeriod;
}

RUNNABLE CTimer::getRunnable()
{
    return m_fnRunnable;
}

void CTimer::stop()
{
    m_bStop = TRUE;
}

BOOL CTimer::isStop()
{
    return m_bStop;
}