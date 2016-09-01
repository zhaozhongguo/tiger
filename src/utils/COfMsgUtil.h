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
*   File Name   : COfMsgUtil.h           *
*   Author      : bnc zgzhao           *
*   Create Date : 2016-6-22           *
*   Version     : 1.0           *
*   Function    : .           *
*                                                                             *
******************************************************************************/
#ifndef __COFMSGUTIL_H
#define __COFMSGUTIL_H
#include "bnc-type.h"
#include "bnc-param.h"
#include "CSmartPtr.h"
#include "openflow-10.h"
#include "openflow-13.h"
#include "openflow-common.h"

class CBuffer;

class COfMsgUtil
{
public:
    static BOOL convertMsgToPacketIn(void* data, packet_in_info_t& packetIn);

    static BOOL sendOfpMsg(INT4 fd, UINT1 ofVersion, UINT1 type, UINT2 length, UINT4 xid);

    static BOOL sendofp13SetConfig(INT4 fd, UINT4 xid);

    static BOOL sendofp13MultiPartRequest(INT4 fd, UINT1* mtp_req);

    static gn_port_t* of13PortConvertter(UINT1 *of_port, gn_port_t *new_sw_port);

    /*
     * 发送Openflow 1.3 Packetout的消息
     */
    static BOOL of13MsgPacketOut(INT4 fd, UINT1 *pktout_req);

    /*
     * 发送Openflow 1.0 Packetout的消息
     */
    static BOOL of10MsgPacketOut(INT4 fd, UINT1 *pktout_req);

    /*
     * 发送Openflow 1.3 修改流表的消息
     */
    static BOOL of13MsgFlowMod(INT4 fd, UINT1 *flowmod_req);
public:
    static std::map<INT4, CSmartPtr<CBuffer> > g_mapSendBuf;
};

#endif
