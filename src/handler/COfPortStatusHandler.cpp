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
*   File Name   : COfPortStatusHandler.cpp           *
*   Author      : bnc zgzhao           *
*   Create Date : 2016-7-6           *
*   Version     : 1.0           *
*   Function    : .           *
*                                                                             *
******************************************************************************/
#include "COfPortStatusHandler.h"
#include "CSwitch.h"
#include "CServer.h"
#include "CTopoMgr.h"
#include "log.h"



COfPortStatusHandler::COfPortStatusHandler()
{
}

COfPortStatusHandler::~COfPortStatusHandler()
{
}

void COfPortStatusHandler::handle(CSmartPtr<CMsgQueue>& queue)
{
    LOG_DEBUG_FMT("%s recv new msg ..., queue size = %lu", toString().c_str(), queue->size());
    UINT1 version = 0;
    CSmartPtr<CMsg> msg;
    while (queue->try_pop(msg))
    {
        version = msg->getVersion();
        if (OFP13_VERSION == version)
        {
            handler13(msg);
        }
        else if (OFP10_VERSION == version)
        {
            handler10(msg);
        }

		
    }
}

std::string COfPortStatusHandler::toString()
{
    return "COfPortStatusHandler";
}

CMsgHandler* COfPortStatusHandler::clone()
{
    CMsgHandler* instance = new COfPortStatusHandler();
    *instance = *this;
    return instance;
}


void COfPortStatusHandler::handler10(CSmartPtr<CMsg>& msg)
{

}

void COfPortStatusHandler::handler13(CSmartPtr<CMsg>& msg)
{
    UINT4 port_state = 0;
    UINT4 port_no = 0;
    gn_port_t new_sw_ports;

    INT4 sockfd = msg->getSockfd();
    const CSmartPtr<CSwitch>& sw = CServer::getInstance()->getSwitchMgr()->findSwBySockFd(sockfd);
	if (sw.isNull())
	{
		return;
	}
    UINT1 *of_msg = msg->getData();
    struct ofp13_port_status *ops = (struct ofp13_port_status *)of_msg;

    port_state = ntohl(ops->desc.state);
    port_no = ntohl(ops->desc.port_no);
	sw->lockPorts();
    std::vector<gn_port_t>& vecPorts = sw->getPorts();
    std::vector<gn_port_t>::iterator iter = vecPorts.begin();
    if (ops->reason == OFPPR_ADD)
    {
        LOG_INFO_FMT("New port found: %s", ops->desc.name);
        COfMsgUtil::of13PortConvertter((UINT1 *)&ops->desc, &new_sw_ports);
        new_sw_ports.stats.max_speed = 1000000;
        for (; iter != vecPorts.end(); ++iter)
        {
            if ((*iter).port_no == port_no){
                (*iter) = new_sw_ports;
				sw->unlockPorts();
                return;
            }
        }
        new_sw_ports.type = PORT_TYPE_NONE;
        vecPorts.push_back(new_sw_ports);
		sw->unlockPorts();
        return;
    }
    else if (ops->reason == OFPPR_DELETE)
    {
        LOG_INFO_FMT("Port deleted: %s", ops->desc.name);
        for (; iter != vecPorts.end(); ++iter)
        {
            if ((*iter).port_no == port_no){
                //删除该端口
                vecPorts.erase(iter);
                sw->unlockPorts();
                //更新所有相邻节点的相邻信息
                UINT8 dpid = sw->getDpid();
                CSmartPtr<CTopoMgr>& topoMgr = CServer::getInstance()->getTopoMgr();
                topoMgr->deleteLine(dpid, port_no);
                topoMgr->updatePaths();
                return;
            }
        }
    }
    else if (ops->reason == OFPPR_MODIFY)
    {
        LOG_INFO_FMT("Port state change: %s[new state: %d]", ops->desc.name, ntohl(ops->desc.state));

        for (; iter != vecPorts.end(); ++iter)
        {
            if ((*iter).port_no == port_no)
            {
                (*iter).state = port_state;
                (*iter).type = PORT_TYPE_NONE;
                sw->unlockPorts();
                if(port_state == OFPPS13_LINK_DOWN || port_state == OFPPS13_BLOCKED)
                {
                    //更新所有相邻节点的相邻信息
                    UINT8 dpid = sw->getDpid();
                    CSmartPtr<CTopoMgr>& topoMgr = CServer::getInstance()->getTopoMgr();
                    topoMgr->deleteLine(dpid, port_no);
                    topoMgr->updatePaths();
                }
				
                return;
            }
        }
    }
	sw->unlockPorts();
}
