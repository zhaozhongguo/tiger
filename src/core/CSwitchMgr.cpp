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
*   File Name   : CSwitchMgr.cpp           *
*   Author      : bnc zgzhao           *
*   Create Date : 2016-5-25           *
*   Version     : 1.0           *
*   Function    : .           *
*                                                                             *
******************************************************************************/
#include "CSwitchMgr.h"
#include "log.h"

static const INT4 DEFAULT_SWITCH_NUM = 80;

CSwitchMgr::CSwitchMgr():m_switchList(DEFAULT_SWITCH_NUM)
{
    m_switchList.clear();
}

CSwitchMgr::~CSwitchMgr()
{
}

void CSwitchMgr::addSw(CSmartPtr<CSwitch>& sw)
{
    if (sw.isNull())
    {
        return;
    }
    m_oMutex.lock();
    m_switchList.push_back(sw);
    m_oMutex.unlock();
}

void CSwitchMgr::delSw(CSmartPtr<CSwitch> sw)
{
    if (sw.isNull())
    {
        return;
    }

    m_oMutex.lock();
    std::list<CSmartPtr<CSwitch> >::iterator iter = m_switchList.begin();
    for (; iter != m_switchList.end(); ++iter)
    {
        if (&(*(*iter)) == &(*sw))
        {
            m_switchList.remove(*iter);
            break;
        }
    }
    m_oMutex.unlock();
}

void CSwitchMgr::delSwBySockFd(INT4 sockfd)
{
    delSw(findSwBySockFd(sockfd));
}

void CSwitchMgr::delSwByDpid(UINT8 dpid)
{
    delSw(findSwByDpid(dpid));
}

void CSwitchMgr::delSwByTag(UINT4 tag)
{
    delSw(findSwByTag(tag));
}


CSmartPtr<CSwitch> CSwitchMgr::findSwBySockFd(INT4 sockfd)
{
    CSmartPtr<CSwitch> ret(NULL);

    m_oMutex.lock();
    std::list<CSmartPtr<CSwitch> >::iterator iter = m_switchList.begin();
    for (; iter != m_switchList.end(); ++iter)
    {
        if ((*iter)->getSockFd() == sockfd)
        {
            ret = *iter;
        }
    }
    m_oMutex.unlock();

    return ret;
}

CSmartPtr<CSwitch> CSwitchMgr::findSwByDpid(UINT8 dpid)
{
    CSmartPtr<CSwitch> ret(NULL);

    m_oMutex.lock();
    std::list<CSmartPtr<CSwitch> >::iterator iter = m_switchList.begin();
    for (; iter != m_switchList.end(); ++iter)
    {
        if ((*iter)->getDpid() == dpid)
        {
            ret = *iter;
        }
    }
    m_oMutex.unlock();

    return ret;
}

CSmartPtr<CSwitch> CSwitchMgr::findSwByTag(UINT4 tag)
{
    CSmartPtr<CSwitch> ret(NULL);

    m_oMutex.lock();
    std::list<CSmartPtr<CSwitch> >::iterator iter = m_switchList.begin();
    for (; iter != m_switchList.end(); ++iter)
    {
        if ((*iter)->getTag() == tag)
        {
            ret = *iter;
        }
    }
    m_oMutex.unlock();

    return ret;
}

std::list<CSmartPtr<CSwitch> > CSwitchMgr::getSwitchList()
{
    return m_switchList;
}