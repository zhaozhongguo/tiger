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
*   File Name   : COfGetConfigReplyHandler.h           *
*   Author      : bnc zgzhao           *
*   Create Date : 2016-7-6           *
*   Version     : 1.0           *
*   Function    : .           *
*                                                                             *
******************************************************************************/
#ifndef _COFGETCONFIGREPLYHANDLER_H
#define _COFGETCONFIGREPLYHANDLER_H
#include "CMsgHandler.h"

/*
 * GetConfigReply消息处理handler类
 *         负责GetConfigReply消息处理
 */
class COfGetConfigReplyHandler: public CMsgHandler
{
public:
    COfGetConfigReplyHandler();
    ~COfGetConfigReplyHandler();

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
};


#endif