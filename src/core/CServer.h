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
*   File Name   : CServer.h           *
*   Author      : bnc zgzhao           *
*   Create Date : 2016-7-1           *
*   Version     : 1.0           *
*   Function    : .           *
*                                                                             *
******************************************************************************/
#ifndef __CSERVER_H
#define __CSERVER_H
#include "bnc-type.h"
#include "CSmartPtr.h"
#include "CRefObj.h"
#include "CConf.h"
#include "CMsgHandlerMgr.h"
#include "CSwitchMgr.h"
#include "CMsgTreeNodeMgr.h"
#include "CTopoMgr.h"
/*
 * 服务器类
 *     管理服务的公共资源
 */
class CServer: public CRefObj
{
public:
    /*
     * 获取RestApi实例
     */
    static CServer* getInstance();

    /*
     * 初始化
     * ret: TRUE: success; FALSE: fail
     */
    BOOL init();

    /**
     * 获取CPU ID
     * ret: 返回CPU ID
     */
    INT4& getCpuId();

    /**
     * 获取配置对象
     * ret: 返回配置对象指针
     */
    CSmartPtr<CConf>& getConf();

    /**
     * 获取handler管理类对象
     * ret: 返回handler管理类对象指针
     */
    CSmartPtr<CMsgHandlerMgr>& getHandlerMgr();

    /**
     * 获取交换机管理类对象
     * ret: 返回交换机管理类对象指针
     */
    CSmartPtr<CSwitchMgr>& getSwitchMgr();

    /**
     * 获取消息存储树
     * ret: 返回消息存储树对象指针
     */
    CSmartPtr<CMsgTreeNodeMgr>& getMsgTreeNodeMgr();

    /**
     * 获取拓扑管理对象
     * ret: 返回拓扑管理对象指针
     */
    CSmartPtr<CTopoMgr>& getTopoMgr();
    
private:
    CServer();
    ~CServer();

private:
    //全局配置对象
    CSmartPtr<CConf> m_pConfPtr;

    //全局CPU id序号
    INT4 m_iCpuId;

    //全局handler mgr对象
    CSmartPtr<CMsgHandlerMgr> m_pHandlerMgrPtr;

    //交换机管理类
    CSmartPtr<CSwitchMgr> m_pSwitchMgrPtr;

    //消息存储数管理类
    CSmartPtr<CMsgTreeNodeMgr> m_pMsgTreeNodeMgrPtr;

    //获取topo管理类
    CSmartPtr<CTopoMgr> m_pTopoMgrPtr;

    //CServer实例
    static CServer* m_pInstance;       
};

#endif