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
#ifndef __CTOPOMGR_H
#define __CTOPOMGR_H
#include "CTimer.h"
#include "bnc-param.h"
#include "bnc-inet.h"
#include "CRefObj.h"
#include "CMutex.h"

class CSwitch;
class CSwitchMgr;

typedef std::map<UINT8, std::list<neighbor_t*>* > CMapNeighbors;
/*
 * 交换机拓扑管理类
 */
class CTopoMgr: public CRefObj
{
public:
    CTopoMgr();
    ~CTopoMgr();
    /*
     * 初始化拓扑管理
     * ret: 成功 TRUE，失败 FALSE
     */
    BOOL init();

    /*
     * 更新拓扑
     * param srcSw: 源交换机
     * param srcPort: 源交换机端口
     * param dstSw: 目的交换机
     * param dstPort: 目的交换机端口
     */
    void updateSwTopo(const CSmartPtr<CSwitch>& srcSw, UINT4 srcPort, const CSmartPtr<CSwitch>& dstSw, UINT4 dstPort);

    /*
     * 删除指定交换机的指定端口与邻居节点的连接
     * param dpid: 交换机dpid
     * param portNo: 交换机端口
     */
    void deleteLine(UINT8 dpid, UINT4 portNo);

    /*
     * 删除指定交换机与邻居节点的连接
     * param dpid: 交换机dpid
     */
    void deleteAllLine(UINT8 dpid);

    /*
     * 更新路径
     */
    void updatePaths();

    /*
     * 打印路径
     */
    void printPaths();
    
private:

    static void createLldpPkt(void *src_addr, UINT8 id, UINT2 port, lldp_t *buffer);
    static void sendLldp(CSwitch& sw, UINT4 port_no, UINT1 *src_addr);
    static void sendLldpToSwitches();
    void updateSwPortType(const CSmartPtr<CSwitch>& srcSw, UINT4 srcPort, const CSmartPtr<CSwitch>& dstSw, UINT4 dstPort);
    
    void clearPaths();
    sw_path_list_t* updateOneSwPath(UINT8 dpid);
    sw_path_t* copyPath(const sw_path_t* path);
    void addNeighborsPath(UINT8 dpid, CSwitchMgr& swUsed, std::stack<sw_path_t*>& pathStack, sw_path_t* curPath);
private:
    CTimer m_oLldpSenderTimer;                   //lldp定时器
    CMapNeighbors m_neighbors;                   //保存拓扑信息
    std::list<sw_path_list_t*> m_totalPathList;  //保存所有路径
    CMutex m_oMutex;
};

#endif