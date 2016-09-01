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
*   File Name   : COfPacketInHandler.h           *
*   Author      : bnc zgzhao           *
*   Create Date : 2016-7-6           *
*   Version     : 1.0           *
*   Function    : .           *
*                                                                             *
******************************************************************************/
#ifndef _COFPACKETINHANDLER_H
#define _COFPACKETINHANDLER_H
#include "CMsgHandler.h"
#include "CServer.h"

/*
 * PacketIn消息处理handler类
 *         负责PacketIn消息处理
 */
class COfPacketInHandler: public CMsgHandler
{
public:
    COfPacketInHandler();
    ~COfPacketInHandler();

    /*
     * 消息处理接口
     * param: queue 待处理消息
     */
    void handle(CSmartPtr<CMsgQueue>& queue);

    /*
     * 获取handler名称
     * ret: 返回handler名称，类型为std::string
     */
    std::string toString();

    /*
     * 克隆方法
     * ret: 返回克隆实例指针
     */
    CMsgHandler* clone();

    /*
     * 取得inport
     */
    UINT4 getInPortNo(packet_in_info_t* packetin) { return packetin->inport; }

    /*
     * 取得交换机信息
     */
    CSmartPtr<CSwitch> & getSw(packet_in_info_t* packetin);

    /*
     * 将数据包转发到特定交换机的特定端口
     */
    void forward(CSmartPtr<CSwitch> & sw, UINT4 portNo, packet_in_info_t* packetin);

    /*
     * 将数据包转发到特定交换机的特定端口
     */
    void forward(const CSmartPtr<CSwitch> & sw, UINT4 port_no, INT4 data_len, void* data);

    /*
     * 泛洪发送
     */
    void flood(const CSmartPtr<CSwitch> & srcSw, packet_in_info_t* packetin);
};


#endif
