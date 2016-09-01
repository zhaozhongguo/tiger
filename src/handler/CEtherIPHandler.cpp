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
*   File Name   : CEtherIPHandler.cpp           *
*   Author      : bnc zgzhao           *
*   Create Date : 2016-7-6           *
*   Version     : 1.0           *
*   Function    : .           *
*                                                                             *
******************************************************************************/
#include "CEtherIPHandler.h"
#include "log.h"
#include "CHostMgr.h"


CEtherIPHandler::CEtherIPHandler()
{
}

CEtherIPHandler::~CEtherIPHandler()
{
}

void CEtherIPHandler::handle(CSmartPtr<CMsgQueue>& queue)
{
    LOG_INFO_FMT("%s recv new msg ..., queue size = %lu", toString().c_str(), queue->size());

    CSmartPtr<CMsg> msg;
    while (queue->try_pop(msg))
    {
		INT4 sockfd = msg->getSockfd();
		const CSmartPtr<CSwitch>& srcSw = CServer::getInstance()->getSwitchMgr()->findSwBySockFd(sockfd);

		packet_in_info_t* packetIn = msg->getPacketIn();
		ip_t *pkt = (ip_t *)packetIn->data;

		// 存储源主机
		CHost* srcHost = CHostMgr::getInstance()->addHost(
				srcSw, 0, packetIn->inport, pkt->eth_head.src, pkt->src);

		// 查找目的主机
		CHost* dstHost = CHostMgr::getInstance()->findHostByIp(pkt->dest);

		// 如果源主机和目的主机都存在, 直接转发
		if ((NULL != srcHost) && (NULL != dstHost))
		{
			forward(dstHost->getSw(), dstHost->getPortNo(), packetIn);
		}
		
		
	}
}

std::string CEtherIPHandler::toString()
{
    return "CEtherIPHandler";
}

CMsgHandler* CEtherIPHandler::clone()
{
    CMsgHandler* instance = new CEtherIPHandler();
    *instance = *this;
    return instance;
}
