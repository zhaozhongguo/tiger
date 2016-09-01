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
*   File Name   : CHost.cpp			*
*   Author      : bnc xflu          *
*   Create Date : 2016-7-21         *
*   Version     : 1.0           	*
*   Function    : .           		*
*                                                                             *
******************************************************************************/
#include "CHost.h"
#include <algorithm>

CHost::CHost()
{
}

CHost::~CHost()
{
}

CHost::CHost(const CSmartPtr<CSwitch> ptrSw, UINT8 dpid, UINT4 portNo, UINT1* mac, UINT4 ip)
{
	m_enType = bnc::host::NORMAL_HOST;
	m_ptrSw = ptrSw;
	m_iDpid = dpid;
	m_iPortNo = portNo;
	memcpy(m_oMac, mac, 6);
	m_oIpList.push_back(ip);
}

BOOL CHost::isIpExist(INT4 ip)
{
	if (m_oIpList.end() != find(m_oIpList.begin(), m_oIpList.end(), ip))
	{
		return TRUE;
	}

	return FALSE;
}
