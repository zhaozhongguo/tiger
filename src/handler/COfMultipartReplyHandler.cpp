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
*   File Name   : COfMultipartReplyHandler.cpp           *
*   Author      : bnc zgzhao           *
*   Create Date : 2016-7-6           *
*   Version     : 1.0           *
*   Function    : .           *
*                                                                             *
******************************************************************************/
#include "COfMultipartReplyHandler.h"
#include "openflow-common.h"
#include "openflow-10.h"
#include "openflow-13.h"
#include "COfMsgUtil.h"
#include "bnc-param.h"
#include "CServer.h"
#include "log.h"



COfMultipartReplyHandler::COfMultipartReplyHandler()
{
}

COfMultipartReplyHandler::~COfMultipartReplyHandler()
{
}

void COfMultipartReplyHandler::handle(CSmartPtr<CMsgQueue>& queue)
{
    LOG_DEBUG_FMT("%s recv new msg ..., queue size = %lu", toString().c_str(), queue->size());
    UINT1 version = 0;
    CSmartPtr<CMsg> msg;
    while (queue->try_pop(msg))
    {
        version = msg->getVersion();
        if (OFP13_VERSION == version)
        {
            handlerMultiReply(msg);
        }

		
    }
}

std::string COfMultipartReplyHandler::toString()
{
    return "COfMultipartReplyHandler";
}

CMsgHandler* COfMultipartReplyHandler::clone()
{
    CMsgHandler* instance = new COfMultipartReplyHandler();
    *instance = *this;
    return instance;
}


void COfMultipartReplyHandler::handlerMultiReply(CSmartPtr<CMsg>& msg)
{
    INT4 sockfd = msg->getSockfd();
    CSmartPtr<CSwitch> sw = CServer::getInstance()->getSwitchMgr()->findSwBySockFd(sockfd);
	if (sw.isNull())
	{
		return;
	}
    gn_port_t new_sw_ports;
    struct ofp_multipart_reply *ofp_mr = (struct ofp_multipart_reply *)msg->getData();
    UINT2 body_len = ntohs(ofp_mr->header.length) - sizeof(struct ofp_multipart_reply);
    int loops = 0;

    if (ntohs(ofp_mr->header.length) < sizeof(*ofp_mr))
    {
        return;
    }

    switch (htons(ofp_mr->type))
    {
        case OFPMP_DESC:
        {
            struct ofp_desc_stats *ods = (struct ofp_desc_stats *)(ofp_mr->body);
            memcpy(&sw->getSwDesc(), ods, sizeof(struct ofp_desc_stats));
            break;
        }
        case OFPMP_PORT_DESC:
        {
            struct ofp13_port *port = (struct ofp13_port *)(ofp_mr->body);
            loops = MAX_PORTS;
            while (body_len && (--loops > 0))
            {
                memset(&new_sw_ports, 0x0, sizeof(new_sw_ports));
                COfMsgUtil::of13PortConvertter((UINT1 *)port, &new_sw_ports);
                new_sw_ports.stats.max_speed = 10737418;
                new_sw_ports.type = PORT_TYPE_NONE;
                body_len -= sizeof(struct ofp13_port);
                port = port + 1;

                if (new_sw_ports.port_no == OFPP13_LOCAL)
                {
                    sw->setLoPort(new_sw_ports);
                    continue;
                }
                //判断是否已经存在
				sw->lockPorts();
				std::vector<gn_port_t>& vecPorts = sw->getPorts();
                std::vector<gn_port_t>::iterator iter = vecPorts.begin();
                for (; iter != vecPorts.end(); ++iter)
                {
                    if ((*iter).port_no == new_sw_ports.port_no){
                        (*iter) = new_sw_ports;
                        break;
                    }
                }

                if (iter == vecPorts.end())
                {
                    vecPorts.push_back(new_sw_ports);
                }
				sw->unlockPorts();
            }
            break;
        }
        case OFPMP_TABLE_FEATURES:
        {
            break;
        }
        case OFPMP_TABLE:
        {
            break;
        }
        case OFPMP_GROUP_FEATURES:
        {
            break;
        }
        case OFPMP_METER_FEATURES:
        {
            break;
        }
        case OFPMP_FLOW:
        {
            break;
        }
        case OFPMP_PORT_STATS:
        {
            break;
        }
        default:
        {
            break;
        }
    }
}
