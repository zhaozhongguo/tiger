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
*   File Name   : COpenstackHost.h		*
*   Author      : bnc xflu          	*
*   Create Date : 2016-7-27         	*
*   Version     : 1.0           		*
*   Function    : .           			*
*                                                                             *
******************************************************************************/
#ifndef _COPENSTACK_HOST_H
#define _COPENSTACK_HOST_H

#include "CHost.h"

/*
 * Openstack主机类
 */
class COpenStackHost : public CHost
{
public:
	/*
	 * 默认构造函数
	 */
	COpenStackHost();

	/*
	 * 默认析构函数
	 */
	~COpenStackHost();

	/*
	 * 带参数的构造函数
	 */
	COpenStackHost(const CSmartPtr<CSwitch> ptrSw,
			UINT8 dpid,
			UINT4 portNo,
			UINT1* mac,
			UINT4 ip,
			const std::string & tenant_id,
			const std::string & network_id,
			const std::string & subnet_id,
			const std::string & port_id) :
			CHost(ptrSw, dpid, portNo, mac, ip),
			m_strTenantId(tenant_id),
			m_strNetworkId(network_id),
			m_strSubnetId(subnet_id),
			m_strPortId(port_id)
	{
		setHostTYpe(bnc::host::OPENSTACK_HOST);;
	}


private:

	std::string m_strTenantId;
	std::string m_strNetworkId;
	std::string m_strSubnetId;
	std::string m_strPortId;
	// security group
};

#endif
