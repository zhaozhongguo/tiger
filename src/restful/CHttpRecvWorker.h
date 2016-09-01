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
*   File Name   : CHttpRecvWorker.h           *
*   Author      : bnc zgzhao           *
*   Create Date : 2016-7-21           *
*   Version     : 1.0           *
*   Function    : .           *
*                                                                             *
******************************************************************************/
#ifndef __CHTTPRECVWORKER__H
#define __CHTTPRECVWORKER__H
#include "CRecvWorker.h"


/**
 * worker类
 *     负责接收并封装消息入队列
 */
class CHttpRecvWorker: public CRecvWorker
{
public:
	/*
	 * 默认构造函数
	 */
    CHttpRecvWorker();

    /*
     * 默认析构函数
     */
    virtual ~CHttpRecvWorker();

    /*
     * 处理消息
     */
    virtual void processMsgIn(INT4 sockfd, CSmartPtr<CBuffer> inMsg);

    /*
     * 拷贝消息
     */
    virtual CRecvWorker* clone();
};




#endif
