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
*   File Name   : COfFeaturesReplyHandler.cpp           *
*   Author      : bnc zgzhao           *
*   Create Date : 2016-7-6           *
*   Version     : 1.0           *
*   Function    : .           *
*                                                                             *
******************************************************************************/
#include "COfFeaturesReplyHandler.h"
#include "CSwitch.h"
#include "CServer.h"
#include "bnc-param.h"
#include "COfMsgUtil.h"
#include "comm-util.h"
#include "openflow-10.h"
#include "openflow-13.h"
#include "openflow-common.h"
#include "log.h"



COfFeaturesReplyHandler::COfFeaturesReplyHandler()
{
}

COfFeaturesReplyHandler::~COfFeaturesReplyHandler()
{
}

void COfFeaturesReplyHandler::handle(CSmartPtr<CMsgQueue>& queue)
{
    LOG_DEBUG_FMT("%s recv new msg ..., queue size = %lu", toString().c_str(), queue->size());
    UINT1 version = 0;
    CSmartPtr<CMsg> msg;
    while (queue->try_pop(msg))
    {
        version = msg->getVersion();
        if (OFP13_VERSION == version)
        {
            handle13(msg);
        }
        else if (OFP10_VERSION == version)
        {
            handle10(msg);
        }

        
    }
}

void COfFeaturesReplyHandler::handle10(CSmartPtr<CMsg>& msg)
{

}

void COfFeaturesReplyHandler::handle13(CSmartPtr<CMsg>& msg)
{
    INT4 sockfd = msg->getSockfd();
    CSmartPtr<CSwitch> sw = CServer::getInstance()->getSwitchMgr()->findSwBySockFd(sockfd);
	if (sw.isNull() || 0 != sw->getDpid())
	{
		return;
	}
	
    UINT1 *of_msg = msg->getData();
    struct ofp13_switch_features *osf = (struct ofp13_switch_features *)of_msg;
    sw->setDpid(ntohll(osf->datapath_id));
    sw->setNBuffers(ntohl(osf->n_buffers));
    sw->setNTables(osf->n_tables);
    sw->setCapabilities(ntohl(osf->capabilities));

    COfMsgUtil::sendofp13SetConfig(sockfd, 0);
    COfMsgUtil::sendOfpMsg(sockfd, OFP13_VERSION, OFPT13_GET_CONFIG_REQUEST, sizeof(struct ofp_header), 0);

    stats_req_info_t stats_req_info;
    stats_req_info.flags = 0;
    stats_req_info.xid = 0;
    stats_req_info.type = OFPMP_DESC;
    COfMsgUtil::sendofp13MultiPartRequest(sockfd, (UINT1*)&stats_req_info);

    stats_req_info.type = OFPMP_PORT_DESC;
    COfMsgUtil::sendofp13MultiPartRequest(sockfd, (UINT1*)&stats_req_info);

    stats_req_info.type = OFPMP_TABLE;
    COfMsgUtil::sendofp13MultiPartRequest(sockfd, (UINT1*)&stats_req_info);

    stats_req_info.type = OFPMP_METER_FEATURES;
    COfMsgUtil::sendofp13MultiPartRequest(sockfd, (UINT1*)&stats_req_info);


    UINT1 dpid[8] = {0};
    INT1 strIP[32] = {0};
    uint8ToStr(sw->getDpid(), dpid);
    LOG_INFO_FMT("New Openflow13 switch [%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x] connected: ip[%s:%d], dpid:%llu", 
        dpid[0], dpid[1], dpid[2], dpid[3], dpid[4], dpid[5], dpid[6], dpid[7], 
        number2ip(sw->getSwIp(), strIP), 
        sw->getSwPort(), 
        sw->getDpid());
}

std::string COfFeaturesReplyHandler::toString()
{
    return "COfFeaturesReplyHandler";
}

CMsgHandler* COfFeaturesReplyHandler::clone()
{
    CMsgHandler* instance = new COfFeaturesReplyHandler();
    *instance = *this;
    return instance;
}
