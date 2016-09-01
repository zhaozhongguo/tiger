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
*   File Name   : CRestManager.cpp	*
*   Author      : bnc xflu          *
*   Create Date : 2016-7-21         *
*   Version     : 1.0           	*
*   Function    : .           		*
*                                                                             *
******************************************************************************/
#include "CRestManager.h"

#include "log.h"

CRestManager* CRestManager::m_pInstance = 0;

CRestManager::CRestManager()
{
}

CRestManager::~CRestManager()
{
}

CRestManager* CRestManager::getInstance()
{
	if (NULL == m_pInstance)
	{
		m_pInstance = new CRestManager();
		m_pInstance->init();
	}

	return m_pInstance;
}

CRestApiMgr* CRestManager::getRestApiMgr()
{
	return CRestApiMgr::getInstance();
}

CRestDefine* CRestManager::getRestDefine()
{
	return CRestDefine::getInstance();
}


BOOL CRestManager::init()
{
	LOG_INFO("initialize Rest Server.");

	if (NULL == CRestApiMgr::getInstance())
	{
		LOG_ERROR("Get RestApi instance failed.");
	}

	if (NULL == CRestDefine::getInstance())
	{
		LOG_ERROR("Get RestDefine instance failed.");
	}

	return TRUE;
}
