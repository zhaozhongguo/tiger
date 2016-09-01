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
*   File Name   : CMsgHandlerMgr.cpp           *
*   Author      : bnc zgzhao           *
*   Create Date : 2016-5-25           *
*   Version     : 1.0           *
*   Function    : .           *
*                                                                             *
******************************************************************************/
#include "CMsgHandlerMgr.h"
#include "COfPacketInHandler.h"
#include "COfHelloHandler.h"
#include "COfErrorHandler.h"
#include "COfEchoRequestHandler.h"
#include "COfEchoReplyHandler.h"
#include "COfExperimenterHandler.h"
#include "COfVendorHandler.h"
#include "COfFeaturesRequestHandler.h"
#include "COfFeaturesReplyHandler.h"
#include "COfGetConfigRequestHandler.h"
#include "COfGetConfigReplyHandler.h"
#include "COfSetConfigHandler.h"
#include "COfPortStatusHandler.h"
#include "COfPortModHandler.h"
#include "COfFlowRemovedHandler.h"
#include "COfFlowModHandler.h"
#include "COfGroupModHandler.h"
#include "COfTableModHandler.h"
#include "COfStatsRequestHandler.h"
#include "COfStatsReplyHandler.h"
#include "COfMultipartRequestHandler.h"
#include "COfMultipartReplyHandler.h"
#include "COfBarrierRequestHandler.h"
#include "COfBarrierReplyHandler.h"
#include "COfQueueGetConfigRequestHandler.h"
#include "COfQueueGetConfigReplyHandler.h"
#include "COfRoleRequestHandler.h"
#include "COfRoleReplyHandler.h"
#include "COfGetAsyncRequestHandler.h"
#include "COfGetAsyncReplyHandler.h"
#include "COfSetAsyncHandler.h"
#include "COfMeterModHandler.h"
#include "COfPacketOutHandler.h"
#include "CEtherLldpHandler.h"
#include "CEtherIPHandler.h"
#include "CEtherIPv6Handler.h"
#include "CEtherArpHandler.h"
#include "CEtherVlanHandler.h"
#include "CIpTcpHandler.h"
#include "CIpUdpHandler.h"
#include "CIpIcmpHandler.h"
#include "CServer.h"
#include "bnc-param.h"
#include "comm-util.h"
#include "log.h"



CMsgHandlerMgr::CMsgHandlerMgr()
{
}

CMsgHandlerMgr::~CMsgHandlerMgr()
{
    stop();
}

BOOL CMsgHandlerMgr::init()
{
    //default handler
    const INT1* key = CServer::getInstance()->getConf()->getConfig("handler", "handler_default_num");
    INT4 number = (NULL == key) ? 1 : atol(key);

    //注册path与handler映射关系
    m_maphandlerTemplate.insert(std::pair<std::string, CSmartPtr<CMsgHandler> >("packet_in", CSmartPtr<CMsgHandler>(new COfPacketInHandler())));
    m_maphandlerTemplate.insert(std::pair<std::string, CSmartPtr<CMsgHandler> >("hello", CSmartPtr<CMsgHandler>(new COfHelloHandler())));
    m_maphandlerTemplate.insert(std::pair<std::string, CSmartPtr<CMsgHandler> >("error", CSmartPtr<CMsgHandler>(new COfErrorHandler())));
    m_maphandlerTemplate.insert(std::pair<std::string, CSmartPtr<CMsgHandler> >("echo_request", CSmartPtr<CMsgHandler>(new COfEchoRequestHandler())));
    m_maphandlerTemplate.insert(std::pair<std::string, CSmartPtr<CMsgHandler> >("echo_reply", CSmartPtr<CMsgHandler>(new COfEchoReplyHandler())));
    m_maphandlerTemplate.insert(std::pair<std::string, CSmartPtr<CMsgHandler> >("experimenter", CSmartPtr<CMsgHandler>(new COfExperimenterHandler())));
    m_maphandlerTemplate.insert(std::pair<std::string, CSmartPtr<CMsgHandler> >("vendor", CSmartPtr<CMsgHandler>(new COfVendorHandler())));
    m_maphandlerTemplate.insert(std::pair<std::string, CSmartPtr<CMsgHandler> >("features_request", CSmartPtr<CMsgHandler>(new COfFeaturesRequestHandler())));
    m_maphandlerTemplate.insert(std::pair<std::string, CSmartPtr<CMsgHandler> >("features_reply", CSmartPtr<CMsgHandler>(new COfFeaturesReplyHandler())));
    m_maphandlerTemplate.insert(std::pair<std::string, CSmartPtr<CMsgHandler> >("get_config_request", CSmartPtr<CMsgHandler>(new COfGetConfigRequestHandler())));
    m_maphandlerTemplate.insert(std::pair<std::string, CSmartPtr<CMsgHandler> >("get_config_reply", CSmartPtr<CMsgHandler>(new COfGetConfigReplyHandler())));
    m_maphandlerTemplate.insert(std::pair<std::string, CSmartPtr<CMsgHandler> >("set_config", CSmartPtr<CMsgHandler>(new COfSetConfigHandler())));
    m_maphandlerTemplate.insert(std::pair<std::string, CSmartPtr<CMsgHandler> >("port_status", CSmartPtr<CMsgHandler>(new COfPortStatusHandler())));
    m_maphandlerTemplate.insert(std::pair<std::string, CSmartPtr<CMsgHandler> >("port_mod", CSmartPtr<CMsgHandler>(new COfPortModHandler())));
    m_maphandlerTemplate.insert(std::pair<std::string, CSmartPtr<CMsgHandler> >("flow_removed", CSmartPtr<CMsgHandler>(new COfFlowRemovedHandler())));
    m_maphandlerTemplate.insert(std::pair<std::string, CSmartPtr<CMsgHandler> >("flow_mod", CSmartPtr<CMsgHandler>(new COfFlowModHandler())));
    m_maphandlerTemplate.insert(std::pair<std::string, CSmartPtr<CMsgHandler> >("group_mod", CSmartPtr<CMsgHandler>(new COfGroupModHandler())));
    m_maphandlerTemplate.insert(std::pair<std::string, CSmartPtr<CMsgHandler> >("table_mod", CSmartPtr<CMsgHandler>(new COfTableModHandler())));
    m_maphandlerTemplate.insert(std::pair<std::string, CSmartPtr<CMsgHandler> >("stats_request", CSmartPtr<CMsgHandler>(new COfStatsRequestHandler())));
    m_maphandlerTemplate.insert(std::pair<std::string, CSmartPtr<CMsgHandler> >("stats_reply", CSmartPtr<CMsgHandler>(new COfStatsReplyHandler())));
    m_maphandlerTemplate.insert(std::pair<std::string, CSmartPtr<CMsgHandler> >("multipart_request", CSmartPtr<CMsgHandler>(new COfMultipartRequestHandler())));
    m_maphandlerTemplate.insert(std::pair<std::string, CSmartPtr<CMsgHandler> >("multipart_reply", CSmartPtr<CMsgHandler>(new COfMultipartReplyHandler())));
    m_maphandlerTemplate.insert(std::pair<std::string, CSmartPtr<CMsgHandler> >("barrier_request", CSmartPtr<CMsgHandler>(new COfBarrierRequestHandler())));
    m_maphandlerTemplate.insert(std::pair<std::string, CSmartPtr<CMsgHandler> >("barrier_reply", CSmartPtr<CMsgHandler>(new COfBarrierReplyHandler())));
    m_maphandlerTemplate.insert(std::pair<std::string, CSmartPtr<CMsgHandler> >("queue_get_config_request", CSmartPtr<CMsgHandler>(new COfQueueGetConfigRequestHandler())));
    m_maphandlerTemplate.insert(std::pair<std::string, CSmartPtr<CMsgHandler> >("queue_get_config_reply", CSmartPtr<CMsgHandler>(new COfQueueGetConfigReplyHandler())));
    m_maphandlerTemplate.insert(std::pair<std::string, CSmartPtr<CMsgHandler> >("role_request", CSmartPtr<CMsgHandler>(new COfRoleRequestHandler())));
    m_maphandlerTemplate.insert(std::pair<std::string, CSmartPtr<CMsgHandler> >("role_reply", CSmartPtr<CMsgHandler>(new COfRoleReplyHandler())));
    m_maphandlerTemplate.insert(std::pair<std::string, CSmartPtr<CMsgHandler> >("get_async_request", CSmartPtr<CMsgHandler>(new COfGetAsyncRequestHandler())));
    m_maphandlerTemplate.insert(std::pair<std::string, CSmartPtr<CMsgHandler> >("get_async_reply", CSmartPtr<CMsgHandler>(new COfGetAsyncReplyHandler())));
    m_maphandlerTemplate.insert(std::pair<std::string, CSmartPtr<CMsgHandler> >("set_async", CSmartPtr<CMsgHandler>(new COfSetAsyncHandler())));
    m_maphandlerTemplate.insert(std::pair<std::string, CSmartPtr<CMsgHandler> >("meter_mod", CSmartPtr<CMsgHandler>(new COfMeterModHandler())));
    m_maphandlerTemplate.insert(std::pair<std::string, CSmartPtr<CMsgHandler> >("packet_out", CSmartPtr<CMsgHandler>(new COfPacketOutHandler())));
    m_maphandlerTemplate.insert(std::pair<std::string, CSmartPtr<CMsgHandler> >("lldp", CSmartPtr<CMsgHandler>(new CEtherLldpHandler())));
    m_maphandlerTemplate.insert(std::pair<std::string, CSmartPtr<CMsgHandler> >("ip", CSmartPtr<CMsgHandler>(new CEtherIPHandler())));
    m_maphandlerTemplate.insert(std::pair<std::string, CSmartPtr<CMsgHandler> >("ipv6", CSmartPtr<CMsgHandler>(new CEtherIPv6Handler())));
    m_maphandlerTemplate.insert(std::pair<std::string, CSmartPtr<CMsgHandler> >("arp", CSmartPtr<CMsgHandler>(new CEtherArpHandler())));
    m_maphandlerTemplate.insert(std::pair<std::string, CSmartPtr<CMsgHandler> >("vlan", CSmartPtr<CMsgHandler>(new CEtherVlanHandler())));
    m_maphandlerTemplate.insert(std::pair<std::string, CSmartPtr<CMsgHandler> >("tcp", CSmartPtr<CMsgHandler>(new CIpTcpHandler())));
    m_maphandlerTemplate.insert(std::pair<std::string, CSmartPtr<CMsgHandler> >("udp", CSmartPtr<CMsgHandler>(new CIpUdpHandler())));
    m_maphandlerTemplate.insert(std::pair<std::string, CSmartPtr<CMsgHandler> >("icmp", CSmartPtr<CMsgHandler>(new CIpIcmpHandler())));

    //根据配置路径生成handler
    CSmartPtr<CConf>& conf = CServer::getInstance()->getConf();
    std::map<std::string, std::string>*  mapTree = conf->getConfig("msg_tree_node");
    if (NULL == mapTree)
    {
        return FALSE;
    }

    std::map<std::string, std::string>::iterator iter = mapTree->begin();
    for (; iter != mapTree->end(); iter++)
    {
        //读取一条配置，eg:ofp13/packe_in/ip/tcp:src_ip,dst_ip
        std::list<std::string> strList;
        split(iter->second, ":", strList);

        //获取路径，eg:ofp13/packe_in/ip/tcp
        std::string pathname = strList.front();
        LOG_DEBUG_FMT("path is : %s", pathname.c_str());

        //获取handler模板
        INT4 pos = pathname.find_last_of("/");
        std::string lastPath = pathname.substr(pos + 1, (pathname).size() - pos - 1);
        std::string firstPath = pathname.substr(0, pathname.find("/"));
        CMap<std::string, CSmartPtr<CMsgHandler> >::iterator iterHandler = m_maphandlerTemplate.find(lastPath);
        if (iterHandler == m_maphandlerTemplate.end() || iterHandler->second.isNull())
        {
            LOG_WARN_FMT("invalid path : %s", lastPath.c_str());
            continue;
        }
        //获取对应的handler列表
        CSmartPtr<CHandlerList> list(NULL);
        CMap<std::string, CSmartPtr<CHandlerList> >::iterator iterList = m_mapHandlers.find(lastPath);
        if (iterList == m_mapHandlers.end() || iterList->second.isNull())
        {
            list = CSmartPtr<CHandlerList>(new CHandlerList());
            m_mapHandlers.insert(std::pair<std::string, CSmartPtr<CHandlerList> >(lastPath, list));
        }
        else
        {
            list = iterList->second;
        }
        //向列表中添加handler
        std::string totalPath = firstPath + "," + lastPath;
        for (INT4 i = 0; i < number; i++)
        {
            CSmartPtr<CMsgHandler> temp(iterHandler->second->clone());
            temp->setPath(totalPath);
            list->push_back(temp);
        }
    }

    LOG_INFO("init CMsgHandlerMgr success");
    return TRUE;
}

void CMsgHandlerMgr::start()
{
    CMap<std::string, CSmartPtr<CHandlerList> >::iterator iter = m_mapHandlers.begin();
    for (; iter != m_mapHandlers.end(); iter++)
    {
        if (!(iter->second).isNull())
        {
            CHandlerList::iterator listIter = iter->second->begin();
            for (; listIter != iter->second->end(); listIter++)
            {
                if (!(*listIter).isNull())
                {
                    (*listIter)->start();
                }
            }
        }
    }
    LOG_INFO("start all handler thread success");
}

void CMsgHandlerMgr::stop()
{
    CMap<std::string, CSmartPtr<CHandlerList> >::iterator iter = m_mapHandlers.begin();
    for (; iter != m_mapHandlers.end(); iter++)
    {
        if (!(iter->second).isNull())
        {
            CHandlerList::iterator listIter = iter->second->begin();
            for (; listIter != iter->second->end(); listIter++)
            {
                if (!(*listIter).isNull())
                {
                    (*listIter)->stop();
                }
            }
        }
    }

    LOG_INFO("stop all handler thread success");
}


void CMsgHandlerMgr::notify(std::string& path)
{
    LOG_DEBUG_FMT("notify msg handler threads, path : %s", path.c_str());
    CMap<std::string, CSmartPtr<CHandlerList> >::iterator iter = m_mapHandlers.find(path);
    if (iter != m_mapHandlers.end())
    {
        if (!(iter->second).isNull())
        {
            CHandlerList::iterator listIter = iter->second->begin();
            for (; listIter != iter->second->end(); listIter++)
            {
                if (!(*listIter).isNull())
                {
                    (*listIter)->notify();
                }
            }
        }
    }
}