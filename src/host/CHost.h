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
*   File Name   : CHost.h			*
*   Author      : bnc xflu          *
*   Create Date : 2016-7-27         *
*   Version     : 1.0           	*
*   Function    : .           		*
*                                                                             *
******************************************************************************/
#ifndef _CHOST_H
#define _CHOST_H

#include <vector>
#include "CSwitch.h"
#include "bnc-type.h"
#include "log.h"
#include "CHostDefine.h"

/*
 * 主机的基类
 */
class CHost
{
public:
	/*
	 * 默认构造函数
	 */
	CHost();

	/*
	 * 默认析构函数
	 */
	~CHost();

	/*
	 * 用于ipv4的带参数的构造函数
	 */
	CHost(const CSmartPtr<CSwitch> ptrSw, UINT8 dpid, UINT4 portNo, UINT1* mac, UINT4 ip);

	/*
	 * 获取主机类型
	 */
	bnc::host::host_type getHostTYpe() { return m_enType; }

	/*
	 * 获取交换机信息
	 */
	CSmartPtr<CSwitch>& getSw() { return m_ptrSw; }

	/*
	 * 获取dpid信息
	 */
	UINT8 getDpid() { return m_iDpid; }

	/*
	 * 获取端口号
	 */
	UINT4 getPortNo() { return m_iPortNo; }

	/*
	 * 获取mac地址
	 */
	UINT1* getMac() { return m_oMac;}

	/*
	 * 获取ipv4地址
	 */
	INT4 getIp() { return m_oIpList[0]; }

	/*
	 * 判断ipv4存在
	 */
	BOOL isIpExist(INT4 ip);

	/*
	 * 设置主机类型
	 */
	void setHostTYpe(bnc::host::host_type type) { m_enType = type; }


	/*
	 * 设置交换机
	 */
	void setSw(CSmartPtr<CSwitch> pSwitch) { m_ptrSw = pSwitch; }

	/*
	 * 设置dpid信息
	 */
	void setDpid(UINT8 dpid) { m_iDpid = dpid; }

	/*
	 * 设置端口号
	 */
	void setPortNo(UINT4 portNo) { m_iPortNo = portNo; }

	/*
	 * 设置mac地址
	 */
	void setMac(UINT1* mac) { memcpy(m_oMac, mac, 6); }

private:

	bnc::host::host_type m_enType;	///< 主机类型
	CSmartPtr<CSwitch> m_ptrSw;		///< 交换机指针
	UINT8 m_iDpid;					///< dpid
	UINT4 m_iPortNo;				///< 端口号
	UINT1 m_oMac[6];				///< mac地址
	std::vector<UINT4> m_oIpList;	/// ipv4 list
	std::vector< std::vector<UINT1> > m_oIpv6List;	///< ipv6 list
};
#endif
