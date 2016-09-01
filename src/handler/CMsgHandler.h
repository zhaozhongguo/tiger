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
*   File Name   : CMsgHandler.h           *
*   Author      : bnc zgzhao           *
*   Create Date : 2016-5-25           *
*   Version     : 1.0           *
*   Function    : .           *
*                                                                             *
******************************************************************************/
#ifndef __CMSGHANDLER_H
#define __CMSGHANDLER_H
#include "CRefObj.h"
#include "CCond.h"
#include "CMutex.h"
#include "CMsg.h"
#include "CSmartPtr.h"
#include "CMsgTreeNode.h"
#include "openflow-common.h"
#include "openflow-10.h"
#include "openflow-13.h"
#include "COfMsgUtil.h"
#include "bnc-type.h"
#include "bnc-inet.h"
#include "memory.h"
#include "CConst.h"
#include "comm-util.h"
#include "CSwitchMgr.h"
#include "CHostMgr.h"
#include "log.h"

/*
 * 消息处理handler类
 *         负责具体的消息处理, 子类必须实现handle接口
 */
class CMsgHandler: public CRefObj
{
public:
    CMsgHandler();
    virtual ~CMsgHandler();

    /*
     * 消息处理接口
     * param: queue 消息队列
     */
    virtual void handle(CSmartPtr<CMsgQueue>& queue) = 0;

    /*
     * 获取handler名称
     * ret: 返回handler名称，类型为std::string
     */
    virtual std::string toString() = 0;
	
    /*
     * 克隆方法
     * ret: 返回克隆实例指针
     */
    virtual CMsgHandler* clone() = 0;

    /*
     * 线程handler启动
     */
    void start();

    /*
     * 停止handler线程
     */
    void stop();

    /*
     * 通知handler线程有新消息
     */
    void notify();

    /*
     * 设置路径
     * param: path 消息路径
     */
    void setPath(std::string& path);

    /*
     * 获取路径
     * ret: 返回消息路径
     */
    std::string& getPath();


    friend void* handlerLoop(void* param);

private:
    pthread_t m_threadId;                    //线程ID
    volatile BOOL m_bStop;                   //线程是否停止
    std::string m_strPath;                   //消息存储路径

    CMutex m_oMutex;
    CCond m_oCond;
};


#endif
