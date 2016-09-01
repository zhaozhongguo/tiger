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
*   File Name   : CHostMgr.cpp		*
*   Author      : bnc xflu          *
*   Create Date : 2016-7-27         *
*   Version     : 1.0           	*
*   Function    : .           		*
*                                                                             *
******************************************************************************/
#include "CHostMgr.h"
#include "memory.h"

CHostMgr* CHostMgr::m_pInstance = 0;

CHostMgr::CHostMgr()
{
}

CHostMgr::~CHostMgr()
{
}

CHostMgr* CHostMgr::getInstance()
{
	if (NULL == m_pInstance) {
		m_pInstance = new CHostMgr();
		m_pInstance->init();
	}

	return (m_pInstance);
}

CHost* CHostMgr::findHostByMac(UINT1* mac)
{
	STL_FOR_LOOP(m_pHostList, iter)
	{
		if (0 == memcmp(mac, (*iter)->getMac(), 6))
		{
			return *iter;
		}
	}

	return NULL;
}

CHost* CHostMgr::findHostByIp(UINT4 ip)
{
	STL_FOR_LOOP(m_pHostList, iter)
	{
		if ((*iter)->isIpExist(ip))
		{
			return *iter;
		}
	}

	return NULL;
}


CHost* CHostMgr::addHost(const CSmartPtr<CSwitch> ptrSw,
		UINT8 dpid, UINT4 portNo, UINT1* mac, UINT4 ip)
{
	CHost* host = findHostByMac(mac);

	if (NULL == host)
	{
		host = new CHost(ptrSw, dpid, portNo, mac, ip);
		m_pHostList.push_back(host);
	}

	return host;
}


CHost* CHostMgr::addHost(const CSmartPtr<CSwitch> ptrSw,
		UINT8 dpid, UINT4 portNo, UINT1* mac, UINT4 ip,
		const std::string & tenant_id,
		const std::string & network_id,
		const std::string & subnet_id,
		const std::string & port_id)
{
	CHost* host = findHostByMac(mac);

	if (NULL == host)
	{
		host = (CHost*)new COpenStackHost(ptrSw, dpid, portNo, mac, ip,
				tenant_id, network_id, subnet_id, port_id);
		m_pHostList.push_back(host);
	}

	return host;

}

