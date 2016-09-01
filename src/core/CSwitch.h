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
*   File Name   : CSwitch.h           *
*   Author      : bnc zgzhao           *
*   Create Date : 2016-6-16           *
*   Version     : 1.0           *
*   Function    : .           *
*                                                                             *
******************************************************************************/
#ifndef __CSWITCH_H
#define __CSWITCH_H
#include "bnc-param.h"
#include "CRefObj.h"
#include "CMutex.h"

#define MAX_PORTS 150

/*
 * 交换机类
 */
class CSwitch: public CRefObj
{
public:
    CSwitch();
    ~CSwitch();

    UINT1 getOfVersion();
    void setOfVersion(UINT1 ofVersion);

    UINT1 getState();
    void setState(UINT1 state);

    UINT4 getTag();
    void setTag(UINT4 tag);

    UINT8 getDpid();
    void setDpid(UINT8 dpid);

    UINT4 getSwIp();
    void setSwIp(UINT4 swIp);

    UINT4 getSwPort();
    void setSwPort(UINT4 swPort);

    INT4 getSockFd();
    void setSockFd(INT4 sockFd);

    UINT4 getNBuffers();
    void setNBuffers(UINT4 buffers);

    UINT4 getNTables();
    void setNTables(UINT4 tables);

    UINT4 getNPorts();
    void setNPorts(UINT4 ports);

    UINT4 getCapabilities();
    void setCapabilities(UINT4 capabilities);

    switch_desc_t& getSwDesc();
    void setSwDesc(switch_desc_t& swDesc);

    gn_port_t& getLoPort();
    void setLoPort(gn_port_t& loPort);

    void lockPorts();
    void unlockPorts();
    std::vector<gn_port_t>& getPorts();

private:
    UINT4 m_uiTag;
    UINT8 m_uiDpid;
    UINT1 m_uiOfVersion;
    UINT4 m_uiSwIp;
    UINT4 m_uiSwPort;
    INT4 m_iSockFd;
    UINT4 m_uiNBuffers;
    UINT4 m_uiNTables;
    UINT4 m_uiNPorts;
    UINT4 m_uiCapabilities;
    switch_desc_t m_oSwDesc;
    gn_port_t m_oLoPort;
    std::vector<gn_port_t> m_vecPorts;
    CMutex m_oPortsMutex;
};

#endif