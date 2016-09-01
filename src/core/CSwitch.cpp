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
*   File Name   : CSwitch.cpp           *
*   Author      : bnc zgzhao           *
*   Create Date : 2016-5-25           *
*   Version     : 1.0           *
*   Function    : .           *
*                                                                             *
******************************************************************************/
#include "CSwitch.h"


CSwitch::CSwitch():
    m_uiTag(0),
    m_uiOfVersion(0),
    m_uiDpid(0),
    m_uiSwIp(0),
    m_uiSwPort(0),
    m_iSockFd(0),
    m_uiNBuffers(0),
    m_uiNTables(0),
    m_uiNPorts(0),
    m_uiCapabilities(0),
    CRefObj(TRUE)
{
    memset(&m_oSwDesc, 0, sizeof(switch_desc_t));
    memset(&m_oLoPort, 0, sizeof(gn_port_t));
}

CSwitch::~CSwitch()
{
}

UINT1 CSwitch::getOfVersion()
{
    return m_uiOfVersion;
}

void CSwitch::setOfVersion(UINT1 ofVersion)
{
    this->m_uiOfVersion = ofVersion;
}

UINT4 CSwitch::getTag()
{
    return m_uiTag;
}

void CSwitch::setTag(UINT4 tag)
{
    this->m_uiTag = tag;
}

UINT8 CSwitch::getDpid() 
{
    return m_uiDpid;
}

void CSwitch::setDpid(UINT8 dpid) 
{
    this->m_uiDpid = dpid;
}

UINT4 CSwitch::getSwIp() 
{
    return m_uiSwIp;
}

void CSwitch::setSwIp(UINT4 swIp) 
{
    this->m_uiSwIp = swIp;
}

UINT4 CSwitch::getSwPort() 
{
    return m_uiSwPort;
}

void CSwitch::setSwPort(UINT4 swPort) 
{
    this->m_uiSwPort = swPort;
}

INT4 CSwitch::getSockFd() 
{
    return m_iSockFd;
}

void CSwitch::setSockFd(INT4 sockFd) 
{
    this->m_iSockFd = sockFd;
}

UINT4 CSwitch::getNBuffers() 
{
    return m_uiNBuffers;
}

void CSwitch::setNBuffers(UINT4 buffers) 
{
    this->m_uiNBuffers = buffers;
}

UINT4 CSwitch::getNTables() 
{
    return m_uiNTables;
}

void CSwitch::setNTables(UINT4 tables) 
{
    this->m_uiNTables = tables;
}

UINT4 CSwitch::getNPorts() 
{
    return m_uiNPorts;
}

void CSwitch::setNPorts(UINT4 ports) 
{
    this->m_uiNPorts = ports;
}

UINT4 CSwitch::getCapabilities() 
{
    return m_uiCapabilities;
}

void CSwitch::setCapabilities(UINT4 capabilities) 
{
    this->m_uiCapabilities = capabilities;
}

switch_desc_t& CSwitch::getSwDesc() 
{
    return m_oSwDesc;
}

void CSwitch::setSwDesc(switch_desc_t& swDesc) 
{
    this->m_oSwDesc = swDesc;
}

gn_port_t& CSwitch::getLoPort() 
{
    return m_oLoPort;
}

void CSwitch::setLoPort(gn_port_t& loPort) 
{
    this->m_oLoPort = loPort;
}

void CSwitch::lockPorts()
{
    m_oPortsMutex.lock();
}

void CSwitch::unlockPorts()
{
    m_oPortsMutex.unlock();
}

std::vector<gn_port_t>& CSwitch::getPorts()
{
    return m_vecPorts;
}