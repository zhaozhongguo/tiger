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
*   File Name   : CMsgTreeNodeMgr.cpp           *
*   Author      : bnc zgzhao           *
*   Create Date : 2016-7-5           *
*   Version     : 1.0           *
*   Function    : .           *
*                                                                             *
******************************************************************************/
#include "CMsgTreeNodeMgr.h"
#include "COfMsgUtil.h"
#include "comm-util.h"
#include "openflow-10.h"
#include "openflow-13.h"
#include "openflow-common.h"
#include "bnc-inet.h"
#include "CServer.h"
#include "COfMsgTreeNode.h"
#include "CEtherMsgTreeNode.h"
#include "CIPMsgTreeNode.h"
#include "log.h"



CMsgTreeNodeMgr::CMsgTreeNodeMgr():m_pRootPtr(new CMsgTreeNode())
{
}

CMsgTreeNodeMgr::~CMsgTreeNodeMgr()
{
}

BOOL CMsgTreeNodeMgr::init()
{
    //初始化路径与TreeNode的映射关系
    CMsgTreeNodeToPath* pMapNodeToPath = new CMsgTreeNodeToPath();
    CMsgTreeNode* pMsgNode = new CMsgTreeNode();
    std::set<std::string>* msgSet = new std::set<std::string>();
    msgSet->insert("ofp10");
    msgSet->insert("ofp13");
    pMapNodeToPath->insert(std::pair<CMsgTreeNode*, std::set<std::string>* >(pMsgNode, msgSet));
    m_nodesToPathList.push_back(pMapNodeToPath);

    pMapNodeToPath = new CMsgTreeNodeToPath();
    pMsgNode = new COfMsgTreeNode();
    msgSet = new std::set<std::string>();
    msgSet->insert("packet_in");
    msgSet->insert("hello");
    msgSet->insert("error");
    msgSet->insert("echo_request");
    msgSet->insert("echo_reply");
    msgSet->insert("experimenter");
    msgSet->insert("vendor");
    msgSet->insert("features_request");
    msgSet->insert("features_reply");
    msgSet->insert("get_config_request");
    msgSet->insert("get_config_reply");
    msgSet->insert("set_config");
    msgSet->insert("port_status");
    msgSet->insert("port_mod");
    msgSet->insert("flow_removed");
    msgSet->insert("flow_mod");
    msgSet->insert("group_mod");
    msgSet->insert("table_mod");
    msgSet->insert("stats_request");
    msgSet->insert("stats_reply");
    msgSet->insert("multipart_request");
    msgSet->insert("multipart_reply");
    msgSet->insert("barrier_request");
    msgSet->insert("barrier_reply");
    msgSet->insert("queue_get_config_request");
    msgSet->insert("queue_get_config_reply");
    msgSet->insert("role_request");
    msgSet->insert("role_reply");
    msgSet->insert("get_async_request");
    msgSet->insert("get_async_reply");
    msgSet->insert("set_async");
    msgSet->insert("meter_mod");
    msgSet->insert("packet_out");
    pMapNodeToPath->insert(std::pair<CMsgTreeNode*, std::set<std::string>* >(pMsgNode, msgSet));
    m_nodesToPathList.push_back(pMapNodeToPath);

    pMapNodeToPath = new CMsgTreeNodeToPath();
    pMsgNode = new CEtherMsgTreeNode();
    msgSet = new std::set<std::string>();
    msgSet->insert("lldp");
    msgSet->insert("ip");
    msgSet->insert("ipv6");
    msgSet->insert("arp");
    msgSet->insert("vlan");
    pMapNodeToPath->insert(std::pair<CMsgTreeNode*, std::set<std::string>* >(pMsgNode, msgSet));
    m_nodesToPathList.push_back(pMapNodeToPath);

    pMapNodeToPath = new CMsgTreeNodeToPath();
    pMsgNode = new CIPMsgTreeNode();
    msgSet = new std::set<std::string>();
    msgSet->insert("tcp");
    msgSet->insert("udp");
    msgSet->insert("icmp");
    pMapNodeToPath->insert(std::pair<CMsgTreeNode*, std::set<std::string>* >(pMsgNode, msgSet));
    m_nodesToPathList.push_back(pMapNodeToPath);

    //初始化路径与消息类型映射关系
    m_mapPathToType.insert(std::pair<std::string, UINT4>("ofp10,ofp10", OFP10_VERSION));
    m_mapPathToType.insert(std::pair<std::string, UINT4>("ofp10,packet_in", OFPT_PACKET_IN));
    m_mapPathToType.insert(std::pair<std::string, UINT4>("ofp10,hello", OFPT_HELLO));
    m_mapPathToType.insert(std::pair<std::string, UINT4>("ofp10,error", OFPT_ERROR));
    m_mapPathToType.insert(std::pair<std::string, UINT4>("ofp10,echo_request", OFPT_ECHO_REQUEST));
    m_mapPathToType.insert(std::pair<std::string, UINT4>("ofp10,echo_reply", OFPT_ECHO_REPLY));
    m_mapPathToType.insert(std::pair<std::string, UINT4>("ofp10,vendor", OFPT_VENDOR));
    m_mapPathToType.insert(std::pair<std::string, UINT4>("ofp10,features_request", OFPT_FEATURES_REQUEST));
    m_mapPathToType.insert(std::pair<std::string, UINT4>("ofp10,features_reply", OFPT_FEATURES_REPLY));
    m_mapPathToType.insert(std::pair<std::string, UINT4>("ofp10,get_config_request", OFPT_GET_CONFIG_REQUEST));
    m_mapPathToType.insert(std::pair<std::string, UINT4>("ofp10,get_config_reply", OFPT_GET_CONFIG_REPLY));
    m_mapPathToType.insert(std::pair<std::string, UINT4>("ofp10,set_config", OFPT_SET_CONFIG));
    m_mapPathToType.insert(std::pair<std::string, UINT4>("ofp10,port_status", OFPT_PORT_STATUS));
    m_mapPathToType.insert(std::pair<std::string, UINT4>("ofp10,port_mod", OFPT_PORT_MOD));
    m_mapPathToType.insert(std::pair<std::string, UINT4>("ofp10,flow_removed", OFPT_FLOW_REMOVED));
    m_mapPathToType.insert(std::pair<std::string, UINT4>("ofp10,flow_mod", OFPT_FLOW_MOD));
    m_mapPathToType.insert(std::pair<std::string, UINT4>("ofp10,stats_request", OFPT_STATS_REQUEST));
    m_mapPathToType.insert(std::pair<std::string, UINT4>("ofp10,stats_reply", OFPT_STATS_REPLY));
    m_mapPathToType.insert(std::pair<std::string, UINT4>("ofp10,barrier_request", OFPT_BARRIER_REQUEST));
    m_mapPathToType.insert(std::pair<std::string, UINT4>("ofp10,barrier_reply", OFPT_BARRIER_REPLY));
    m_mapPathToType.insert(std::pair<std::string, UINT4>("ofp10,queue_get_config_request", OFPT_QUEUE_GET_CONFIG_REQUEST));
    m_mapPathToType.insert(std::pair<std::string, UINT4>("ofp10,queue_get_config_reply", OFPT_QUEUE_GET_CONFIG_REPLY));
    m_mapPathToType.insert(std::pair<std::string, UINT4>("ofp10,packet_out", OFPT_PACKET_OUT));
    m_mapPathToType.insert(std::pair<std::string, UINT4>("ofp10,lldp", ETHER_LLDP));
    m_mapPathToType.insert(std::pair<std::string, UINT4>("ofp10,ip", ETHER_IP));
    m_mapPathToType.insert(std::pair<std::string, UINT4>("ofp10,ipv6", ETHER_IPV6));
    m_mapPathToType.insert(std::pair<std::string, UINT4>("ofp10,arp", ETHER_ARP));
    m_mapPathToType.insert(std::pair<std::string, UINT4>("ofp10,vlan", ETHER_VLAN));
    m_mapPathToType.insert(std::pair<std::string, UINT4>("ofp10,tcp", IPPROTO_TCP));
    m_mapPathToType.insert(std::pair<std::string, UINT4>("ofp10,udp", IPPROTO_UDP));
    m_mapPathToType.insert(std::pair<std::string, UINT4>("ofp10,icmp", IPPROTO_ICMP));

    m_mapPathToType.insert(std::pair<std::string, UINT4>("ofp13,ofp13", OFP13_VERSION));
    m_mapPathToType.insert(std::pair<std::string, UINT4>("ofp13,packet_in", OFPT13_PACKET_IN));
    m_mapPathToType.insert(std::pair<std::string, UINT4>("ofp13,hello", OFPT13_HELLO));
    m_mapPathToType.insert(std::pair<std::string, UINT4>("ofp13,error", OFPT13_ERROR));
    m_mapPathToType.insert(std::pair<std::string, UINT4>("ofp13,echo_request", OFPT13_ECHO_REQUEST));
    m_mapPathToType.insert(std::pair<std::string, UINT4>("ofp13,echo_reply", OFPT13_ECHO_REPLY));
    m_mapPathToType.insert(std::pair<std::string, UINT4>("ofp13,experimenter", OFPT13_EXPERIMENTER));
    m_mapPathToType.insert(std::pair<std::string, UINT4>("ofp13,features_request", OFPT13_FEATURES_REQUEST));
    m_mapPathToType.insert(std::pair<std::string, UINT4>("ofp13,features_reply", OFPT13_FEATURES_REPLY));
    m_mapPathToType.insert(std::pair<std::string, UINT4>("ofp13,get_config_request", OFPT13_GET_CONFIG_REQUEST));
    m_mapPathToType.insert(std::pair<std::string, UINT4>("ofp13,get_config_reply", OFPT13_GET_CONFIG_REPLY));
    m_mapPathToType.insert(std::pair<std::string, UINT4>("ofp13,set_config", OFPT13_SET_CONFIG));
    m_mapPathToType.insert(std::pair<std::string, UINT4>("ofp13,port_status", OFPT13_PORT_STATUS));
    m_mapPathToType.insert(std::pair<std::string, UINT4>("ofp13,port_mod", OFPT13_PORT_MOD));
    m_mapPathToType.insert(std::pair<std::string, UINT4>("ofp13,flow_removed", OFPT13_FLOW_REMOVED));
    m_mapPathToType.insert(std::pair<std::string, UINT4>("ofp13,flow_mod", OFPT13_FLOW_MOD));
    m_mapPathToType.insert(std::pair<std::string, UINT4>("ofp13,group_mod", OFPT13_GROUP_MOD));
    m_mapPathToType.insert(std::pair<std::string, UINT4>("ofp13,table_mod", OFPT13_TABLE_MOD));
    m_mapPathToType.insert(std::pair<std::string, UINT4>("ofp13,multipart_request", OFPT13_MULTIPART_REQUEST));
    m_mapPathToType.insert(std::pair<std::string, UINT4>("ofp13,multipart_reply", OFPT13_MULTIPART_REPLY));
    m_mapPathToType.insert(std::pair<std::string, UINT4>("ofp13,barrier_request", OFPT13_BARRIER_REQUEST));
    m_mapPathToType.insert(std::pair<std::string, UINT4>("ofp13,barrier_reply", OFPT13_BARRIER_REPLY));
    m_mapPathToType.insert(std::pair<std::string, UINT4>("ofp13,queue_get_config_request", OFPT13_QUEUE_GET_CONFIG_REQUEST));
    m_mapPathToType.insert(std::pair<std::string, UINT4>("ofp13,queue_get_config_reply", OFPT13_QUEUE_GET_CONFIG_REPLY));
    m_mapPathToType.insert(std::pair<std::string, UINT4>("ofp13,role_request", OFPT13_ROLE_REQUEST));
    m_mapPathToType.insert(std::pair<std::string, UINT4>("ofp13,role_reply", OFPT13_ROLE_REPLY));
    m_mapPathToType.insert(std::pair<std::string, UINT4>("ofp13,get_async_request", OFPT13_GET_ASYNC_REQUEST));
    m_mapPathToType.insert(std::pair<std::string, UINT4>("ofp13,get_async_reply", OFPT13_GET_ASYNC_REPLY));
    m_mapPathToType.insert(std::pair<std::string, UINT4>("ofp13,set_async", OFPT13_SET_ASYNC));
    m_mapPathToType.insert(std::pair<std::string, UINT4>("ofp13,meter_mod", OFPT13_METER_MOD));
    m_mapPathToType.insert(std::pair<std::string, UINT4>("ofp13,packet_out", OFPT13_PACKET_OUT));
    m_mapPathToType.insert(std::pair<std::string, UINT4>("ofp13,lldp", ETHER_LLDP));
    m_mapPathToType.insert(std::pair<std::string, UINT4>("ofp13,ip", ETHER_IP));
    m_mapPathToType.insert(std::pair<std::string, UINT4>("ofp13,ipv6", ETHER_IPV6));
    m_mapPathToType.insert(std::pair<std::string, UINT4>("ofp13,arp", ETHER_ARP));
    m_mapPathToType.insert(std::pair<std::string, UINT4>("ofp13,vlan", ETHER_VLAN));
    m_mapPathToType.insert(std::pair<std::string, UINT4>("ofp13,tcp", IPPROTO_TCP));
    m_mapPathToType.insert(std::pair<std::string, UINT4>("ofp13,udp", IPPROTO_UDP));
    m_mapPathToType.insert(std::pair<std::string, UINT4>("ofp13,icmp", IPPROTO_ICMP));

    //根据配置生成消息树
    geneMsgTree();

    LOG_INFO("init CMsgTreeNodeMgr success");
    return TRUE;
}


CSmartPtr<CMsgTreeNode>& CMsgTreeNodeMgr::getRootNode()
{
    return m_pRootPtr;
}


CSmartPtr<CMsgTreeNode> CMsgTreeNodeMgr::getMsgTreeNodeByPath(std::string& path)
{
    CSmartPtr<CMsgTreeNode> ret(NULL);
    std::map<std::string, CSmartPtr<CMsgTreeNode> >::iterator iter = m_mapPathToNode.find(path);
    if (iter != m_mapPathToNode.end())
    {
        ret =  iter->second;
    }

    return ret;
}


void CMsgTreeNodeMgr::geneMsgTree()
{
    LOG_INFO("begin to analyse msg tree node config");
    CSmartPtr<CConf>& conf = CServer::getInstance()->getConf();
    std::map<std::string, std::string>*  mapTree = conf->getConfig("msg_tree_node");
    if (NULL == mapTree)
    {
        return;
    }

    std::map<std::string, std::string>::iterator iter = mapTree->begin();
    std::list<std::string> strList;
    std::list<std::string>::iterator iterStrList;
    std::list<CMsgTreeNodeToPath*>::iterator iterPathList;
    for (; iter != mapTree->end(); iter++)
    {
        CSmartPtr<CMsgTreeNode> childPtr(NULL);
        CSmartPtr<CMsgTreeNode> fatherPtr = m_pRootPtr;

        //读取一条配置，eg:ofp13/packe_in/ip/tcp:src_ip,dst_ip
        strList.clear();
        split(iter->second, ":", strList);
        LOG_DEBUG_FMT("total path is : %s", iter->second.c_str());

        //获取路径，eg:ofp13/packe_in/ip/tcp
        std::string pathname = strList.front();
        strList.pop_front();
        LOG_DEBUG_FMT("path is : %s", pathname.c_str());

        //获取子队列关键字, eg: src_ip,dst_ip
        std::string subQueueKey = "";
        if (strList.size() > 0)
        {
            subQueueKey = strList.front();
            strList.pop_front();
            LOG_DEBUG_FMT("subQueueKey is : %s", subQueueKey.c_str());
        }

        //解析路径
        strList.clear();
        split(pathname, "/", strList);
        UINT4 pathLen = strList.size();
        if (0 == pathLen)
        {
            LOG_WARN_FMT("invalid pathname : %s", pathname.c_str());
            continue;
        }

        //判断路径中的每个关键字是否合法，并创建对应的CMsgTreeNode对象
        iterStrList = strList.begin();
        iterPathList = m_nodesToPathList.begin();

        //获取协议版本
        std::string& verison = *iterStrList;
        for (UINT4 i = 1; iterStrList != strList.end() && iterPathList != m_nodesToPathList.end(); iterStrList++, iterPathList++, i++)
        {
            //判断该路径是否已经存在
            childPtr = fatherPtr->findChildNode(*iterStrList);
            if (childPtr.isNull())
            {
                LOG_DEBUG_FMT("path is not exist : %s", (*iterStrList).c_str());
                //判断路径是否合法
                CMsgTreeNodeToPath::iterator iterNodeToPath = (*iterPathList)->begin();
                for (; iterNodeToPath != (*iterPathList)->end(); iterNodeToPath++)
                {
                    if (iterNodeToPath->second->end() != iterNodeToPath->second->find(*iterStrList))
                    {
                        LOG_DEBUG_FMT("add new msg tree node, father path : %s, child path : %s", fatherPtr->getNodePath().c_str(), (*iterStrList).c_str());
                        //路径合法，则创建treeNode
                        childPtr = iterNodeToPath->first->clone();
                        childPtr->setNodePath(*iterStrList);
                        INT4 type = m_mapPathToType.find(verison + "," + *iterStrList)->second;
                        childPtr->setMsgType(type);
                        if (i == pathLen)
                        {
                            childPtr->setSubQueueKey(subQueueKey);
                        }
                        fatherPtr->addChildNode(childPtr);
                        //添加路径与节点的映射关系
                        m_mapPathToNode.insert(std::pair<std::string, CSmartPtr<CMsgTreeNode> >(verison + "," + *iterStrList, childPtr));
                        break;
                    }
                }

                //非法路径
                if (iterNodeToPath == (*iterPathList)->end())
                {
                    LOG_WARN_FMT("invalid pathname = %s", (*iterStrList).c_str());
                    break;
                }
            }
            else
            {
                LOG_DEBUG_FMT("pathname = %s is already exist", (*iterStrList).c_str());
            }

            fatherPtr = childPtr;
        }
    }
    LOG_INFO("success to analyse msg tree node config");
}