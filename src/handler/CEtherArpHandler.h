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
*   File Name   : CEtherArpHandler.h           *
*   Author      : bnc zgzhao           *
*   Create Date : 2016-7-6           *
*   Version     : 1.0           *
*   Function    : .           *
*                                                                             *
******************************************************************************/
#ifndef _CETHERARPHANDLER_H
#define _CETHERARPHANDLER_H
#include "COfPacketInHandler.h"

/*
 * arp消息处理handler类
 *         负责arp消息处理
 */
class CEtherArpHandler: public COfPacketInHandler
{
public:
    CEtherArpHandler();
    ~CEtherArpHandler();

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
     * 判断是不是request包
     */
    static BOOL isRequest(arp_t* arp_pkt);

    /*
     * 判断是不是reply包
     */
    static BOOL isReply(arp_t* arp_pkt);

    /*
     * 创建一个arp包
     */
    static BOOL create_arp_pkt(arp_t* arp_pkt, UINT2 code,
    		UINT1* src_mac, UINT1* dst_mac, UINT4 src_ip, UINT4 dst_ip);

    /*
     * 创建arp reply包
     */
    static BOOL create_arp_reply_pkt(arp_t* arp_pkt, CHost* src, CHost* dst);

    /*
     * 创建arp flood包
     */
    static BOOL create_arp_request_flood_pkt(arp_t* arp_pkt, CHost* src, UINT4 dst_ip);
};


#endif
