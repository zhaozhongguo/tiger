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
*   File Name   : CSwitchMgr.h           *
*   Author      : bnc zgzhao           *
*   Create Date : 2016-6-16           *
*   Version     : 1.0           *
*   Function    : .           *
*                                                                             *
******************************************************************************/
#ifndef __CSWITCHMGR_H
#define __CSWITCHMGR_H
#include "CSwitch.h"
#include "CSmartPtr.h"
#include "CMutex.h"

/*
 * 交换机管理类
 */
class CSwitchMgr: public CRefObj
{
public:
    CSwitchMgr();
    ~CSwitchMgr();

    void addSw(CSmartPtr<CSwitch>& sw);

    void delSw(CSmartPtr<CSwitch> sw);
    void delSwBySockFd(INT4 sockfd);
    void delSwByDpid(UINT8 dpid);
    void delSwByTag(UINT4 tag);

    CSmartPtr<CSwitch> findSwBySockFd(INT4 sockfd);
    CSmartPtr<CSwitch> findSwByDpid(UINT8 dpid);
    CSmartPtr<CSwitch> findSwByTag(UINT4 tag);

    std::list<CSmartPtr<CSwitch> > getSwitchList();
    
private:
    std::list<CSmartPtr<CSwitch> > m_switchList;
    CMutex m_oMutex;
};

#endif