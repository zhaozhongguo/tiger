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
*   File Name   : CServer.cpp           *
*   Author      : bnc zgzhao           *
*   Create Date : 2016-7-1           *
*   Version     : 1.0           *
*   Function    : .           *
*                                                                             *
******************************************************************************/
#include "CServer.h"
#include "log.h"

CServer* CServer::m_pInstance = NULL;

CServer* CServer::getInstance()
{
    if (NULL == m_pInstance) 
    {
        m_pInstance = new CServer();
        if (FALSE == m_pInstance->init())
        {
            exit(0);
        }
    }

    return (m_pInstance);
}

BOOL CServer::init()
{
    try
    {
        m_pConfPtr = CSmartPtr<CConf>(new CConf());
        m_pHandlerMgrPtr = CSmartPtr<CMsgHandlerMgr>(new CMsgHandlerMgr());
        m_pSwitchMgrPtr = CSmartPtr<CSwitchMgr>(new CSwitchMgr());
        m_pMsgTreeNodeMgrPtr = CSmartPtr<CMsgTreeNodeMgr>(new CMsgTreeNodeMgr());
        m_pTopoMgrPtr = CSmartPtr<CTopoMgr>(new CTopoMgr());
    }
    catch (...)
    {
        LOG_ERROR("CServer init failure");
        return FALSE;
    }

    return TRUE;
}

CServer::CServer():
    m_iCpuId(0)
{
}

CServer::~CServer()
{
}

INT4& CServer::getCpuId()
{
    return m_iCpuId;
}

CSmartPtr<CConf>& CServer::getConf()
{
    return m_pConfPtr;
}

CSmartPtr<CMsgHandlerMgr>& CServer::getHandlerMgr()
{
    return m_pHandlerMgrPtr;
}

CSmartPtr<CSwitchMgr>& CServer::getSwitchMgr()
{
    return m_pSwitchMgrPtr;
}

CSmartPtr<CMsgTreeNodeMgr>& CServer::getMsgTreeNodeMgr()
{
    return m_pMsgTreeNodeMgrPtr;
}

CSmartPtr<CTopoMgr>& CServer::getTopoMgr()
{
    return m_pTopoMgrPtr;
}