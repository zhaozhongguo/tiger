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
*   File Name   : CHostMgr.h		*
*   Author      : bnc xflu          *
*   Create Date : 2016-7-27         *
*   Version     : 1.0           	*
*   Function    : .           		*
*                                                                             *
******************************************************************************/
#ifndef _CHOSTMGR_H
#define _CHOSTMGR_H

#include "CHost.h"
#include "COpenStackHost.h"
#include <list>

/*
 * 管理主机的类
 */
class CHostMgr
{
public:
	/*
	 * 获取实例
	 */
	static CHostMgr* getInstance();

	/*
	 * 增加主机
	 */
	CHost* addHost(const CSmartPtr<CSwitch> ptrSw,
			UINT8 dpid, UINT4 portNo, UINT1* mac, UINT4 ip);

	/*
	 * 增加Openstack主机
	 */
	CHost* addHost(const CSmartPtr<CSwitch> ptrSw,
			UINT8 dpid, UINT4 portNo, UINT1* mac, UINT4 ip,
			const std::string & tenant_id,
			const std::string & network_id,
			const std::string & subnet_id,
			const std::string & port_id);

	/*
	 * 增加主机
	 */
	BOOL addHost(CHost* host);

	/*
	 * 通过mac地址查找主机
	 */
	CHost* findHostByMac(UINT1* mac);

	/*
	 * 通过ip查找主机
	 */
	CHost* findHostByIp(UINT4 ip);

	/*
	 * 通过ipv6查找主机
	 */
	CHost* findHostByIpv6(UINT1* ipv6);

	/*
	 * 通过交换机和端口号查找主机
	 */
	CHost* findHostBySwPort(CSwitch* sw, UINT4 portNo);

	/*
	 * 获取主机list
	 */
	std::list<CHost*> getHostList() { return m_pHostList; }

private:
	/*
	 * 默认构造函数
	 */
	CHostMgr();

	/*
	 * 默认析构函数
	 */
	~CHostMgr();

	/*
	 * 初始化函数
	 */
	void init() { }

	std::list<CHost*> m_pHostList;		///< 主机list

	static CHostMgr* m_pInstance;		///< 实例
};

#endif
