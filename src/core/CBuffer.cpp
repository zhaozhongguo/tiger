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
*   File Name   : CBuffer.cpp           *
*   Author      : bnc zgzhao           *
*   Create Date : 2016-5-25           *
*   Version     : 1.0           *
*   Function    : .           *
*                                                                             *
******************************************************************************/
#include "CBuffer.h"
#include "bnc-error.h"
#include "CMsg.h"
#include "log.h"

INT4 RECV_BUF_SIZE = 131072;

CBuffer::CBuffer():m_iCur(0),m_iSize(RECV_BUF_SIZE),m_iTail(0)
{
    m_strData = new UINT1[RECV_BUF_SIZE];
}

CBuffer::CBuffer(INT4 size):m_iCur(0),m_iSize(size),m_iTail(0)
{
    m_strData = new UINT1[size];
}

CBuffer::~CBuffer()
{
    delete[] m_strData;
}

UINT1* CBuffer::write(UINT1* data, INT4 len)
{
    if (len <= m_iSize)
    {
        //剩余空间够用
        if ((m_iSize - m_iTail) >= len)
        {
            memcpy(m_strData + m_iTail, data, len);
            m_iTail += len;
        }
        else
        {
            //缓冲区满，丢弃缓冲区消息
            LOG_WARN("recv buffer is full !");
            m_iCur = 0;
            m_iTail = 0;
        }
    }
    else
    {
        LOG_WARN("over flow !");
    }

    return m_strData;
}

INT4 CBuffer::size()
{
    return m_iSize;
}

INT4 CBuffer::current()
{
    return m_iCur;
}

INT4 CBuffer::tail()
{
    return m_iTail;
}

UINT1* CBuffer::getData()
{
    return m_strData;
}

void CBuffer::clear()
{
    memset(m_strData, 0, m_iSize);
    m_iCur = 0;
    m_iTail = 0;
}

CSmartPtr<CMsg> CBuffer::getMsg()
{
    if (m_iTail <= m_iCur)
    {
        return CSmartPtr<CMsg>(NULL);
    }
    
    UINT1* ofMsg = m_strData + m_iCur;
    struct ofp_header *header = (struct ofp_header*)ofMsg;
    INT4 packetLen = ntohs(header->length);
    INT4 headLen = sizeof(struct ofp_header);
    INT4 availableLen = m_iTail - m_iCur;
    if (availableLen < headLen || availableLen < packetLen)
    {
        memmove(m_strData, ofMsg, availableLen);
        m_iCur = 0;
        m_iTail = availableLen;
        return CSmartPtr<CMsg>(NULL);
    }

    CSmartPtr<CMsg> msg(new CMsg(ofMsg, packetLen));
    if (GN_OK != msg->check())
    {
        //缓冲区数据损坏，丢弃消息
        m_iCur = 0;
        m_iTail = 0;
        return CSmartPtr<CMsg>(NULL);
    }

    msg->setVersion(header->version);
    msg->setOfType(header->type);
    m_iCur += packetLen;
    if (m_iCur >= m_iTail)
    {
        m_iCur = 0;
        m_iTail = 0;
    }

    return msg;
}


CMutex* CBuffer::getMutex()
{
    return &m_oMutex;
}