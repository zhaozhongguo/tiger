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
*   File Name   : CTopoMgr.h           *
*   Author      : bnc zgzhao           *
*   Create Date : 2016-7-21           *
*   Version     : 1.0           *
*   Function    : .           *
*                                                                             *
******************************************************************************/
#include "CTopoMgr.h"
#include "CServer.h"
#include "openflow-common.h"
#include "openflow-10.h"
#include "openflow-13.h"
#include "CSwitch.h"
#include "COfMsgUtil.h"
#include "comm-util.h"
#include "log.h"



//lldp发送周期
static const INT4 LLDP_SEND_PERIOD = 5000;

void CTopoMgr::createLldpPkt(void *src_addr, UINT8 id, UINT2 port, lldp_t *buffer)
{
    UINT1 dest_addr[6] = {0x01,0x80,0xc2,0x00,0x00,0x0e};

    memcpy(buffer->eth_head.dest,dest_addr,6);
    memcpy(buffer->eth_head.src, src_addr, 6);
    buffer->eth_head.proto = htons(0x88cc);

    buffer->chassis_tlv_type_and_len = htons(0x0209);
    buffer->chassis_tlv_subtype = LLDP_CHASSIS_ID_LOCALLY_ASSIGNED;
    buffer->chassis_tlv_id = htonll(id);   //datapath id

    buffer->port_tlv_type_and_len    = htons(0x0403);
    buffer->port_tlv_subtype = LLDP_PORT_ID_COMPONENT;
    buffer->port_tlv_id = htons(port);         //send port

    buffer->ttl_tlv_type_and_len = htons(0x0602);
    buffer->ttl_tlv_ttl = htons(120);

    buffer->endof_lldpdu_tlv_type_and_len = 0x00;
}

void CTopoMgr::sendLldp(CSwitch& sw, UINT4 port_no, UINT1 *src_addr)
{
    packout_req_info_t packout_req_info;
    lldp_t lldp_pkt;

    packout_req_info.buffer_id = 0xffffffff;
    packout_req_info.inport = 0xfffffffd;
    packout_req_info.outport = port_no;
    packout_req_info.max_len = 0xff;
    packout_req_info.xid = 0;
    packout_req_info.data_len = sizeof(lldp_t);
    packout_req_info.data = (UINT1 *)&lldp_pkt;

    /* create lldp packet */
    createLldpPkt(src_addr, sw.getDpid(), port_no, &lldp_pkt);
    if(sw.getOfVersion() == OFP10_VERSION)
    {
        COfMsgUtil::of10MsgPacketOut(sw.getSockFd(), (UINT1*)&packout_req_info);
        LOG_DEBUG_FMT("send of10 lldp to sw, dpid = %llu, port no = %u", sw.getDpid(), port_no);
    }
    else if(sw.getOfVersion() == OFP13_VERSION)
    {
        COfMsgUtil::of13MsgPacketOut(sw.getSockFd(), (UINT1*)&packout_req_info);
        LOG_DEBUG_FMT("send of13 lldp to sw, dpid = %llu, port no = %u", sw.getDpid(), port_no);
    }
}

void CTopoMgr::sendLldpToSwitches()
{
    std::list<CSmartPtr<CSwitch> > swList = CServer::getInstance()->getSwitchMgr()->getSwitchList();
    std::list<CSmartPtr<CSwitch> >::iterator iter = swList.begin();
    std::vector<gn_port_t>::iterator iterPort;
    for (; iter != swList.end(); ++iter)
    {
        CSmartPtr<CSwitch>& sw = *iter;
        if (sw.isNull())
        {
            continue;
        }
		sw->lockPorts();
        std::vector<gn_port_t>& ports = sw->getPorts();
        iterPort = ports.begin();
        for (; iterPort != ports.end(); ++iterPort)
        {
            sendLldp(*sw, (*iterPort).port_no, (*iterPort).hw_addr);
        }
		sw->unlockPorts();
    }
} 

CTopoMgr::CTopoMgr()
{
}

CTopoMgr::~CTopoMgr()
{
}

BOOL CTopoMgr::init()
{
    m_oLldpSenderTimer.schedule(CTopoMgr::sendLldpToSwitches, LLDP_SEND_PERIOD, LLDP_SEND_PERIOD);
    LOG_INFO("init CTopoMgr success");

    return TRUE;
}



void CTopoMgr::updateSwTopo(
    const CSmartPtr<CSwitch>& srcSw,
    UINT4 srcPort,
    const CSmartPtr<CSwitch>& dstSw,
    UINT4 dstPort)
{
    m_oMutex.lock();
    UINT8 srcDpid = srcSw->getDpid();
    LOG_DEBUG_FMT("recv lldp reply, src_sw:%llu, src_port_no:%u, dst_sw:%llu, dst_port_no:%u",
            srcDpid, srcPort, dstSw->getDpid(), dstPort);
    CMapNeighbors::iterator srcIter = m_neighbors.find(srcDpid);
    std::list<neighbor_t*>* ll = NULL;
    neighbor_t* neigh = NULL;
    if (srcIter == m_neighbors.end())
    {
        neigh = new neighbor_t();
        neigh->src_sw = srcSw;
        neigh->dst_sw = dstSw;
        neigh->src_port_no = srcPort;
        neigh->dst_port_no = dstPort;
        ll = new std::list<neighbor_t*>();
        ll->push_back(neigh);
        m_neighbors.insert(std::pair<UINT8, std::list<neighbor_t*>* >(srcDpid, ll));
        LOG_INFO_FMT("add new neighbor, src_sw:%llu, src_port_no:%u, dst_sw:%llu, dst_port_no:%u",
            srcDpid, srcPort, dstSw->getDpid(), dstPort);
        updateSwPortType(srcSw, srcPort, dstSw, dstPort);
        m_oMutex.unlock();
        updatePaths();
        return;
    }
    else
    {
        ll = srcIter->second;
        std::list<neighbor_t*>::iterator dstIter = ll->begin();
        for (; dstIter != ll->end(); dstIter++)
        {
            if (!((*dstIter)->dst_sw.isNull()) && 
                (*dstIter)->dst_sw->getDpid() == dstSw->getDpid() &&
                (*dstIter)->dst_port_no == dstPort)
            {
                break;
            }
        }

        //不存在则添加
        if (dstIter == ll->end())
        {
            neigh = new neighbor_t();
            neigh->src_sw = srcSw;
            neigh->dst_sw = dstSw;
            neigh->src_port_no = srcPort;
            neigh->dst_port_no = dstPort;
            ll->push_back(neigh);
            LOG_INFO_FMT("add new neighbor, src_sw:%llu, src_port_no:%u, dst_sw:%llu, dst_port_no:%u",
                srcDpid, srcPort, dstSw->getDpid(), dstPort);
            updateSwPortType(srcSw, srcPort, dstSw, dstPort);
            m_oMutex.unlock();
            updatePaths();
            return;
        }
    }
    m_oMutex.unlock();
}


void CTopoMgr::deleteLine(UINT8 dpid, UINT4 portNo)
{
    m_oMutex.lock();
    //获取本交换机所有相连节点
    CMapNeighbors::iterator srcIter = m_neighbors.find(dpid);
    if (srcIter == m_neighbors.end())
    {
        m_oMutex.unlock();
        return;
    }

    std::list<neighbor_t*>* ll = srcIter->second;
    if (NULL == ll)
    {
        m_oMutex.unlock();
        return;
    }

    //获取与该端口相连的交换机dpid，并删除该相邻节点信息
    UINT8 neighDpid = 0;
    UINT4 neighPortNo = 0;
    std::list<neighbor_t*>::iterator dstIter = ll->begin();
    for (; dstIter != ll->end(); dstIter++)
    {
        if ((*dstIter)->src_port_no == portNo)
        {
            if (!((*dstIter)->dst_sw.isNull()))
            {
                neighDpid = (*dstIter)->dst_sw->getDpid();
                neighPortNo = (*dstIter)->dst_port_no;
                delete *dstIter;
                ll->erase(dstIter);
                if (ll->empty())
                {
                    delete ll;
                    m_neighbors.erase(dpid);
                }
                LOG_INFO_FMT("delete line, dpid:%llu, port no:%u", dpid, portNo);
                break;
            }
        }
    }
    m_oMutex.unlock();

    if (0 != neighDpid)
    {
        //删除相邻节点中关于本节点的信息
        deleteLine(neighDpid, neighPortNo);
    }
}

void CTopoMgr::deleteAllLine(UINT8 dpid)
{
    const CSmartPtr<CSwitch>& sw = CServer::getInstance()->getSwitchMgr()->findSwByDpid(dpid);
    if (sw.isNull())
    {
        return;
    }
	sw->lockPorts();
    std::vector<gn_port_t>& ports = sw->getPorts();
    INT4 size = ports.size();
    for (INT4 i = 0; i < size; ++i)
    {
        deleteLine(dpid, ports[i].port_no);
    }
	sw->unlockPorts();
}


void CTopoMgr::updateSwPortType(
    const CSmartPtr<CSwitch>& srcSw,
    UINT4 srcPort,
    const CSmartPtr<CSwitch>& dstSw,
    UINT4 dstPort)
{
	srcSw->lockPorts();
    std::vector<gn_port_t>& srcPorts = srcSw->getPorts();
    INT4 size = srcPorts.size();
    for (INT4 i = 0; i < size; ++i)
    {
        if (srcPorts[i].port_no == srcPort)
        {
            srcPorts[i].type = PORT_TYPE_SWITCH;
            break;
        }
    }
	srcSw->unlockPorts();

	dstSw->lockPorts();
    std::vector<gn_port_t>& dstPorts = dstSw->getPorts();
    size = dstPorts.size();
    for (INT4 i = 0; i < size; ++i)
    {
        if (dstPorts[i].port_no == dstPort)
        {
            dstPorts[i].type = PORT_TYPE_SWITCH;
            break;
        }
    }
	dstSw->unlockPorts();
}

void CTopoMgr::updatePaths()
{
    m_oMutex.lock();
    //清空旧数据
    clearPaths();
    //生成所有路径
    std::list<CSmartPtr<CSwitch> > swList = CServer::getInstance()->getSwitchMgr()->getSwitchList();
    std::list<CSmartPtr<CSwitch> >::iterator iter = swList.begin();
    sw_path_list_t* pPathList = NULL;
    for (; iter != swList.end(); ++iter)
    {
        CSmartPtr<CSwitch>& sw = *iter;
        if (sw.isNull())
        {
            continue;
        }

        pPathList = updateOneSwPath(sw->getDpid());
        if (NULL != pPathList)
        {
            m_totalPathList.push_back(pPathList);
        }
    }
    m_oMutex.unlock();
}

sw_path_list_t* CTopoMgr::updateOneSwPath(UINT8 dpid)
{
    //判断是否合法
    CSmartPtr<CSwitch> sw = CServer::getInstance()->getSwitchMgr()->findSwByDpid(dpid);
    if (sw.isNull())
    {
        return NULL;
    }

    //添加自己到自己的路径
    sw_path_node_t* pSwPathNode = new sw_path_node_t();
    pSwPathNode->sw = sw;
    pSwPathNode->port_no = 0;

    sw_path_t* pSwPath = new sw_path_t();
    pSwPath->src_sw = sw;
    pSwPath->dst_sw = sw;
    pSwPath->node_list.push_back(pSwPathNode);

    sw_path_list_t* pSwPathList = new sw_path_list_t();
    pSwPathList->dst_sw = sw;

    //使用过的交换机放入swUsed
    CSwitchMgr swUsed;
    swUsed.addSw(sw);

    //建立路径栈
    std::stack<sw_path_t*> pathStack;
    pathStack.push(pSwPath);

    //添加相邻节点到该节点路径
    while (pathStack.size())
    {
        //弹出一条路径，获取源sw
        sw_path_t* curPath = pathStack.top();
        pathStack.pop();
        CSmartPtr<CSwitch>& srcSw = curPath->src_sw;

        //寻找srcSw的所有相邻节点
        addNeighborsPath(srcSw->getDpid(), swUsed, pathStack, curPath);

        //路径加入list
        pSwPathList->path_list.push_back(curPath);
    }

    return pSwPathList;
}

void CTopoMgr::addNeighborsPath(
    UINT8 dpid,
    CSwitchMgr& swUsed,
    std::stack<sw_path_t*>& pathStack,
    sw_path_t* curPath)
{
    CMapNeighbors::iterator iterNeighs = m_neighbors.find(dpid);
    if (iterNeighs == m_neighbors.end())
    {
        return;
    }

    std::list<neighbor_t*>* pList = iterNeighs->second;
    if (NULL == pList)
    {
        return;
    }

    std::list<neighbor_t*>::iterator iter = pList->begin();
    neighbor_t* neigh = NULL;
    for (; iter != pList->end(); ++iter)
    {
        neigh = *iter;
        if (NULL == neigh)
        {
            continue;
        }

        CSmartPtr<CSwitch>& neighSw = neigh->dst_sw;
        if (neighSw.isNull())
        {
            continue;
        }

        //判断是否已经存在
        if (!(swUsed.findSwByDpid(neighSw->getDpid())).isNull())
        {
            continue;
        }

        //新建路径并入栈
        sw_path_node_t* node = new sw_path_node_t();
        node->sw = neighSw;
        node->port_no = neigh->dst_port_no;
        sw_path_t* newPath = copyPath(curPath);
        newPath->node_list.push_front(node);
        newPath->src_sw = neighSw;
        pathStack.push(newPath);

        //标记为已使用
        swUsed.addSw(neighSw);
    }

}

sw_path_t* CTopoMgr::copyPath(const sw_path_t* path)
{
    sw_path_t* newPath = new sw_path_t();
    newPath->src_sw = path->src_sw;
    newPath->dst_sw = path->dst_sw;

    std::list<sw_path_node_t*>::const_iterator iterNode = path->node_list.begin();
    for (; iterNode != path->node_list.end(); ++iterNode)
    {
        sw_path_node_t* node = new sw_path_node_t();
        node->sw = (*iterNode)->sw;
        node->port_no = (*iterNode)->port_no;
        newPath->node_list.push_back(node);
    }

    return newPath;
}


void CTopoMgr::clearPaths()
{
    std::list<sw_path_list_t*>::iterator iterPathList = m_totalPathList.begin();
    for (; iterPathList != m_totalPathList.end(); ++iterPathList)
    {
        sw_path_list_t* pPathList = *iterPathList;
        if (NULL == pPathList)
        {
            continue;
        }

        std::list<sw_path_t*>::iterator iterPath = pPathList->path_list.begin();
        for (; iterPath != pPathList->path_list.end(); ++iterPath)
        {
            sw_path_t* pPath = *iterPath;
            if (NULL == pPath)
            {
                continue;
            }

            std::list<sw_path_node_t*>::iterator iterNode = pPath->node_list.begin();
            for (; iterNode != pPath->node_list.end(); ++iterNode)
            {
                if (NULL == *iterNode)
                {
                    delete *iterNode;
                }
            }
            pPath->node_list.clear();
            delete pPath;
        }
        pPathList->path_list.clear();
        delete pPathList;
    }
    m_totalPathList.clear();
}

void CTopoMgr::printPaths()
{
    m_oMutex.lock();
    std::list<sw_path_list_t*>::iterator iterPathList = m_totalPathList.begin();
    for (; iterPathList != m_totalPathList.end(); ++iterPathList)
    {
        sw_path_list_t* pPathList = *iterPathList;
        if (NULL == pPathList)
        {
            continue;
        }
        LOG_INFO_FMT("---------------PathList dst sw:%llu---------------", pPathList->dst_sw->getDpid());

        std::list<sw_path_t*>::iterator iterPath = pPathList->path_list.begin();
        for (; iterPath != pPathList->path_list.end(); ++iterPath)
        {
            sw_path_t* pPath = *iterPath;
            if (NULL == pPath)
            {
                continue;
            }

            LOG_INFO_FMT("PATH src sw:%llu, dst sw:%llu", pPath->src_sw->getDpid(), pPath->dst_sw->getDpid());

            std::list<sw_path_node_t*>::iterator iterNode = pPath->node_list.begin();
            for (; iterNode != pPath->node_list.end(); ++iterNode)
            {
                if (NULL != *iterNode)
                {
                    LOG_INFO_FMT("      Node sw:%llu, port_no:%u", (*iterNode)->sw->getDpid(), (*iterNode)->port_no);
                }
            }
        }
    }
    m_oMutex.unlock();
}