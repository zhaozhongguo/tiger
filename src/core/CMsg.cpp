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
*   File Name   : CMsg.cpp           *
*   Author      : bnc zgzhao           *
*   Create Date : 2016-5-25           *
*   Version     : 1.0           *
*   Function    : .           *
*                                                                             *
******************************************************************************/
#include "CMsg.h"
#include "bnc-error.h"
#include "CRefObj.h"
#include "log.h"

CMsg::CMsg(UINT1* data, INT4 len):
    m_iLen(len),
    m_iSockfd(-1),
    m_uiofType(0),
    m_uiVersion(0xFF),
    m_pPacketIn(NULL),
    m_iState(OF_MSG_STATE_NEW),
    CRefObj(TRUE)
{
    m_strData = new UINT1[len];
    memset(m_strData, 0 , len);
    memcpy(m_strData, data, len);
}

CMsg::~CMsg()
{
    if (m_strData)
    {
        delete[] m_strData;
    }

    if (m_pPacketIn)
    {
        delete m_pPacketIn;
    }
}

UINT1* CMsg::getData()
{
    return m_strData;
}

INT4 CMsg::getSockfd()
{
    return m_iSockfd;
}

void CMsg::setSockfd(INT4 sockfd)
{
    m_iSockfd = sockfd;
}

INT4 CMsg::length()
{
    return m_iLen;
}

INT4 CMsg::check()
{
    struct ofp_header* oh = (struct ofp_header*)m_strData;
    if(oh->version == OFP10_VERSION)
    {
        if(oh->type >= OFP10_MAX_MSG)
        {
            LOG_ERROR_FMT("bad message type:%d", oh->type);
            return GN_ERR;
        }
    }
    else if(oh->version == OFP13_VERSION)
    {
        if(oh->type >= OFP13_MAX_MSG)
        {
            LOG_ERROR_FMT("bad message type:%d", oh->type);
            return GN_ERR;
        }
    }
    else
    {
        LOG_ERROR_FMT("bad openflow version:%d", oh->version);
        return GN_ERR;
    }

    return GN_OK;
}


UINT1 CMsg::getVersion()
{
    return m_uiVersion;
}

void CMsg::setVersion(UINT1 version)
{
    m_uiVersion = version;
}

UINT1 CMsg::getOfType()
{
    return m_uiofType;
}

void CMsg::setOfType(UINT1 type)
{
    m_uiofType = type;
}


INT4 CMsg::getState()
{
    return m_iState;
}

void CMsg::setState(INT4 state)
{
    m_iState = state;
}

packet_in_info_t* CMsg::getPacketIn()
{
    if (NULL == m_pPacketIn)
    {
        m_pPacketIn = new packet_in_info_t();
    }

    return m_pPacketIn;
}