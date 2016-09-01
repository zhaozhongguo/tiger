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
*   File Name   : CMsgHandlerMgr.h           *
*   Author      : bnc zgzhao           *
*   Create Date : 2016-5-25           *
*   Version     : 1.0           *
*   Function    : .           *
*                                                                             *
******************************************************************************/
#ifndef __CMSGHANDLERMGR_H
#define __CMSGHANDLERMGR_H
#include "bnc-type.h"
#include "CSmartPtr.h"
#include "CRefObj.h"
#include "CMap.h"
#include "CMsgHandler.h"
#include "CList.h"


typedef CList<CSmartPtr<CMsgHandler> > CHandlerList;

/*
 * handler管理类
 */
class CMsgHandlerMgr: public CRefObj
{
public:
    CMsgHandlerMgr();
    ~CMsgHandlerMgr();

    /*
     * 初始化
     * ret: 成功 TRUE，失败 FALSE
     */
    BOOL init();
    
    /*
     * 启动所有handler
     */
    void start();

    /*
     * 停止所有handler
     */
    void stop();

    /*
     * 通知handler有新消息
     */
    void notify(std::string& path);

private:
    CMap<std::string, CSmartPtr<CHandlerList> > m_mapHandlers;
    CMap<std::string, CSmartPtr<CMsgHandler> > m_maphandlerTemplate;
};


#endif