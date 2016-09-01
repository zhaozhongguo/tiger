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
*   File Name   : COfPacketInHandler.cpp           *
*   Author      : bnc zgzhao           *
*   Create Date : 2016-7-6           *
*   Version     : 1.0           *
*   Function    : .           *
*                                                                             *
******************************************************************************/
#include "COfPacketInHandler.h"

COfPacketInHandler::COfPacketInHandler()
{
}

COfPacketInHandler::~COfPacketInHandler()
{
}

void COfPacketInHandler::handle(CSmartPtr<CMsgQueue>& queue)
{
    LOG_INFO_FMT("%s recv new msg ..., queue size = %lu", toString().c_str(), queue->size());
    queue->clear();
}

std::string COfPacketInHandler::toString()
{
    return "COfPacketInHandler";
}

CMsgHandler* COfPacketInHandler::clone()
{
    CMsgHandler* instance = new COfPacketInHandler();
    *instance = *this;
    return instance;
}

void COfPacketInHandler::forward(CSmartPtr<CSwitch> & sw, UINT4 port_no, packet_in_info_t* packetin)
{
	LOG_IF_VOID (sw.isNull() || (0 == port_no) || (NULL == packetin),
			     "fail to forward packet. input parameter invalid.");

	packout_req_info_t packout_req_info;

	packout_req_info.buffer_id = 0xffffffff;
	packout_req_info.inport = OFPP13_CONTROLLER;
	packout_req_info.outport = port_no;
    packout_req_info.max_len = 0xff;
	packout_req_info.xid = 0;
	packout_req_info.data_len = packetin->data_len;
	packout_req_info.data = (UINT1 *)packetin->data;

	if(sw->getOfVersion() == OFP10_VERSION)
	{
		COfMsgUtil::of10MsgPacketOut(sw->getSockFd(), (UINT1*)&packout_req_info);
		LOG_INFO("send of10 arp");
	}
	else if(sw->getOfVersion() == OFP13_VERSION)
	{
		COfMsgUtil::of13MsgPacketOut(sw->getSockFd(), (UINT1*)&packout_req_info);
		LOG_INFO("send of13 arp");
	}
}

void COfPacketInHandler::forward(const CSmartPtr<CSwitch> & sw, UINT4 port_no, INT4 data_len, void* data)
{
	packout_req_info_t packout_req_info;

	packout_req_info.buffer_id = 0xffffffff;
	packout_req_info.inport = OFPP13_CONTROLLER;
	packout_req_info.outport = port_no;
    packout_req_info.max_len = 0xff;
	packout_req_info.xid = 0;
	packout_req_info.data_len = data_len + sizeof(packout_req_info_t);
	packout_req_info.data = (UINT1 *)data;

	if(sw->getOfVersion() == OFP10_VERSION)
	{
		COfMsgUtil::of10MsgPacketOut(sw->getSockFd(), (UINT1*)&packout_req_info);
		LOG_INFO("send of10 arp");
	}
	else if(sw->getOfVersion() == OFP13_VERSION)
	{
		COfMsgUtil::of13MsgPacketOut(sw->getSockFd(), (UINT1*)&packout_req_info);
		LOG_INFO("send of13 arp");
	}
}

void COfPacketInHandler::flood(const CSmartPtr<CSwitch> & srcSw, packet_in_info_t* packetin)
{
	packout_req_info_t packout_req_info;

	packout_req_info.buffer_id = 0xffffffff;
	packout_req_info.inport = OFPP13_CONTROLLER;
	// packout_req_info.outport = port_no;
	packout_req_info.max_len = 0xff;
	packout_req_info.xid = 0;
	packout_req_info.data_len = packetin->data_len;
	packout_req_info.data = (UINT1 *)packetin->data;

	std::list<CSmartPtr<CSwitch> > swList = CServer::getInstance()->getSwitchMgr()->getSwitchList();

	STL_FOR_LOOP(swList, iter)
	{
		(*iter)->lockPorts();
		std::vector<gn_port_t>& ports = (*iter)->getPorts();

		STL_FOR_LOOP(ports, portIter)
		{
			// LOG_INFO(portIter->port_no);

			if (PORT_TYPE_SWITCH == portIter->type)
			{
				continue;
			}

			if (((*iter)->getDpid() == srcSw->getDpid())
				&& (portIter->port_no == packetin->inport))
			{
				continue;
			}

			packout_req_info.outport = portIter->port_no;

			if (OFP10_VERSION == iter->getPtr()->getOfVersion())
			{
				COfMsgUtil::of10MsgPacketOut(iter->getPtr()->getSockFd(), (UINT1*)&packout_req_info);
				LOG_INFO("send of10 packet");
			}
			else if (OFP13_VERSION == iter->getPtr()->getOfVersion())
			{
				COfMsgUtil::of13MsgPacketOut(iter->getPtr()->getSockFd(), (UINT1*)&packout_req_info);
				LOG_INFO("send of13 packet");
			}
		}
		(*iter)->unlockPorts();
	}
}
