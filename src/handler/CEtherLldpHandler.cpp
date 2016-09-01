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
*   File Name   : CEtherLldpHandler.cpp           *
*   Author      : bnc zgzhao           *
*   Create Date : 2016-7-6           *
*   Version     : 1.0           *
*   Function    : .           *
*                                                                             *
******************************************************************************/
#include "CEtherLldpHandler.h"
#include "bnc-param.h"
#include "bnc-inet.h"
#include "CTopoMgr.h"
#include "CServer.h"
#include "comm-util.h"
#include "log.h"



CEtherLldpHandler::CEtherLldpHandler()
{
}

CEtherLldpHandler::~CEtherLldpHandler()
{
}

void CEtherLldpHandler::handle(CSmartPtr<CMsgQueue>& queue)
{
    LOG_DEBUG_FMT("%s recv new msg ..., queue size = %lu", toString().c_str(), queue->size());
    INT4 sockfd = 0;
    UINT8 neighborDpid = 0;
    UINT4 neighborPortNo = 0;
    CSmartPtr<CMsg> msg;
    while (queue->try_pop(msg))
    {
        packet_in_info_t* packetIn = msg->getPacketIn();
        lldp_t *pkt = (lldp_t *)packetIn->data;
        if (pkt->chassis_tlv_subtype != LLDP_CHASSIS_ID_LOCALLY_ASSIGNED||
            pkt->port_tlv_subtype != LLDP_PORT_ID_COMPONENT ||
            pkt->ttl_tlv_ttl != htons(120))
        {
			
            continue;
        }

        neighborDpid = ntohll(pkt->chassis_tlv_id);
        sockfd = msg->getSockfd();
        const CSmartPtr<CSwitch>& srcSw = CServer::getInstance()->getSwitchMgr()->findSwBySockFd(sockfd);
        const CSmartPtr<CSwitch>& dstSw = CServer::getInstance()->getSwitchMgr()->findSwByDpid(neighborDpid);
        if (srcSw.isNull() || dstSw.isNull())
        {
			
            continue;
        }

        neighborPortNo = ntohs(pkt->port_tlv_id);
        CSmartPtr<CTopoMgr>& topoMgr = CServer::getInstance()->getTopoMgr();
        topoMgr->updateSwTopo(srcSw, packetIn->inport, dstSw, neighborPortNo);
        
    }
}

std::string CEtherLldpHandler::toString()
{
    return "CEtherLldpHandler";
}

CMsgHandler* CEtherLldpHandler::clone()
{
    CMsgHandler* instance = new CEtherLldpHandler();
    *instance = *this;
    return instance;
}
