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
*   File Name   : COfHelloHandler.cpp           *
*   Author      : bnc zgzhao           *
*   Create Date : 2016-7-6           *
*   Version     : 1.0           *
*   Function    : .           *
*                                                                             *
******************************************************************************/
#include "COfHelloHandler.h"
#include "openflow-common.h"
#include "openflow-10.h"
#include "openflow-13.h"
#include "COfMsgUtil.h"
#include "CServer.h"
#include "log.h"



COfHelloHandler::COfHelloHandler()
{
}

COfHelloHandler::~COfHelloHandler()
{
}

void COfHelloHandler::handle(CSmartPtr<CMsgQueue>& queue)
{
    LOG_DEBUG_FMT("%s recv new msg ..., queue size = %lu", toString().c_str(), queue->size());
    UINT1 version = 0;
    INT4 sockfd = 0;
    CSmartPtr<CMsg> msg;
    while (queue->try_pop(msg))
    {
        sockfd = msg->getSockfd();
        version = msg->getVersion();
        const CSmartPtr<CSwitch>& sw = CServer::getInstance()->getSwitchMgr()->findSwBySockFd(sockfd);
		if (sw.isNull())
		{
			
			return;
		}
        sw->setOfVersion(version);
        if (OFP13_VERSION == version)
        {
            COfMsgUtil::sendOfpMsg(sockfd, version, OFPT13_HELLO, sizeof(struct ofp_hello), 0);
            COfMsgUtil::sendOfpMsg(sockfd, version, OFPT13_FEATURES_REQUEST, sizeof(struct ofp_header), 0);
            LOG_DEBUG("send of13 hello msg to switch");
            LOG_DEBUG("send of13 feature request msg to switch");
        }
        else if (OFP10_VERSION == version)
        {
            COfMsgUtil::sendOfpMsg(sockfd, version, OFPT_HELLO, sizeof(struct ofp_hello), 0);
            COfMsgUtil::sendOfpMsg(sockfd, version, OFPT_FEATURES_REQUEST, sizeof(struct ofp_header), 0);
            LOG_DEBUG("send of10 hello msg to switch");
            LOG_DEBUG("send of10 feature request msg to switch");
        }

        
    }
}

std::string COfHelloHandler::toString()
{
    return "COfHelloHandler";
}

CMsgHandler* COfHelloHandler::clone()
{
    CMsgHandler* instance = new COfHelloHandler();
    *instance = *this;
    return instance;
}