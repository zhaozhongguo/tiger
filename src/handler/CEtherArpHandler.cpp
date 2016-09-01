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
*   File Name   : CEtherArpHandler.cpp           *
*   Author      : bnc zgzhao           *
*   Create Date : 2016-7-6           *
*   Version     : 1.0           *
*   Function    : .           *
*                                                                             *
******************************************************************************/
#include "CEtherArpHandler.h"

UINT1 BROADCAST_MAC[6] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff};

CEtherArpHandler::CEtherArpHandler()
{
}

CEtherArpHandler::~CEtherArpHandler()
{
}

BOOL CEtherArpHandler::isRequest(arp_t* arp_pkt)
{
	LOG_IF_RETURN (NULL == arp_pkt, FALSE, "arp pointer is NULL");

	BOOL value = (arp_pkt->opcode == htons(bnc::constant::ARP_CODE_REQUEST)) ? TRUE : FALSE;
	return value;
}

BOOL CEtherArpHandler::isReply(arp_t* arp_pkt)
{
	LOG_IF_RETURN (NULL == arp_pkt, FALSE, "arp pointer is NULL");

	BOOL value = (arp_pkt->opcode == htons(bnc::constant::ARP_CODE_REPLY)) ? TRUE : FALSE;
	return value;
}

BOOL CEtherArpHandler::create_arp_pkt(arp_t* arp_pkt, UINT2 code,
		UINT1* src_mac, UINT1* dst_mac, UINT4 src_ip, UINT4 dst_ip)
{
	LOG_IF_RETURN ((NULL == arp_pkt) || (NULL == src_mac) || (NULL == dst_mac),
				   FALSE, "create arp packet failed. pointer is NULL.");

	memset(arp_pkt, sizeof(arp_t), 0);

	memcpy(arp_pkt->eth_head.src, src_mac, 6);
	memcpy(arp_pkt->eth_head.dest, dst_mac, 6);
	arp_pkt->eth_head.proto = htons(ETHER_ARP);
	arp_pkt->hardwaretype = htons(1);
	arp_pkt->prototype = htons(ETHER_IP);
	arp_pkt->hardwaresize = 0x6;
	arp_pkt->protocolsize = 0x4;
	arp_pkt->opcode = htons(code);
	arp_pkt->sendip = src_ip;
	arp_pkt->targetip= dst_ip;
	memcpy(arp_pkt->sendmac, src_mac, 6);
	memcpy(arp_pkt->targetmac, dst_mac, 6);

	return TRUE;
}

BOOL CEtherArpHandler::create_arp_reply_pkt(arp_t* arp_pkt, CHost* src, CHost* dst)
{
	LOG_IF_RETURN ((NULL == arp_pkt) || (NULL == src) || (NULL == dst),
			       FALSE, "create arp reply packet failed. pointer is NULL.");

	create_arp_pkt(arp_pkt, bnc::constant::ARP_CODE_REPLY,
			dst->getMac(), src->getMac(), dst->getIp(), src->getIp());

	return TRUE;
}

BOOL CEtherArpHandler::create_arp_request_flood_pkt(arp_t* arp_pkt, CHost* src, UINT4 dst_ip)
{
	LOG_IF_RETURN ((NULL == arp_pkt) || (NULL == src),
			       FALSE, "create arp reply packet failed. pointer is NULL.");

	create_arp_pkt(arp_pkt, bnc::constant::ARP_CODE_REQUEST,
			src->getMac(), BROADCAST_MAC, src->getIp(), dst_ip);

	return TRUE;
}


void CEtherArpHandler::handle(CSmartPtr<CMsgQueue>& queue)
{
    LOG_INFO_FMT("%s recv new msg ..., queue size = %lu", toString().c_str(), queue->size());

    CSmartPtr<CMsg> msg;
    while (queue->try_pop(msg))
    {
    	INT4 sockfd = msg->getSockfd();
    	const CSmartPtr<CSwitch>& srcSw = CServer::getInstance()->getSwitchMgr()->findSwBySockFd(sockfd);

    	// 获取packetin 信息
    	packet_in_info_t* packetIn = msg->getPacketIn();
    	arp_t* pkt = (arp_t*)packetIn->data;

		// 保存源主机信息
		CHost* srcHost = CHostMgr::getInstance()->addHost(srcSw, 0, packetIn->inport, pkt->sendmac, pkt->sendip);

		// 查找目标主机
		CHost* dstHost = CHostMgr::getInstance()->findHostByIp(pkt->targetip);

		// 如果源主机和目标主机都存在
		if ((NULL != srcHost) && (NULL != dstHost))
		{
		    arp_t arp_pkt;

			// 如果是Arp Request
			if (isRequest(pkt))
			{
				LOG_INFO("Create Arp Reply");
				// 创建Arp Reply回应报文并且转发
				create_arp_reply_pkt(&arp_pkt, srcHost, dstHost);
				forward(srcSw, srcHost->getPortNo(), sizeof(arp_pkt), &arp_pkt);
			}

			// 如果是Arp Reply
			if (isReply(pkt))
			{
				LOG_INFO("forward Arp Reply");
				// 转发Arp Reply回应报文
				forward(dstHost->getSw(), dstHost->getPortNo(),  sizeof(arp_pkt), &arp_pkt);
			}
		}
		// 如果不存在
		else
		{
			LOG_INFO("flood Arp request");
			// 将这个包广播
			// flood(packetIn);
		}
		
		
    }
}

std::string CEtherArpHandler::toString()
{
    return "CEtherArpHandler";
}

CMsgHandler* CEtherArpHandler::clone()
{
    CMsgHandler* instance = new CEtherArpHandler();
    *instance = *this;
    return instance;
}
