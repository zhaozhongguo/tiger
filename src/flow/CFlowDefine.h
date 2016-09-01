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
*   File Name   : CFlowDefine.h		*
*   Author      : bnc xflu          *
*   Create Date : 2016-8-1          *
*   Version     : 1.0           	*
*   Function    : .           		*
*                                                                             *
******************************************************************************/
#ifndef _CFLOWDEFINE_H
#define _CFLOWDEFINE_H
#include "bnc-type.h"
#include "log.h"

/*
 * 定义了一些流表所需要使用的定义
 */

namespace bnc
{
	namespace flow
	{
		#define FABRIC_IMPL_HARD_TIME_OUT 0
		#define FABRIC_IMPL_IDLE_TIME_OUT 0
		#define FABRIC_ARP_HARD_TIME_OUT 0
		#define FABRIC_ARP_IDLE_TIME_OUT 100
		#define FABRIC_FIND_HOST_IDLE_TIME_OUT 100

		#define FABRIC_PRIORITY_HOST_INPUT_FLOW 5
		#define FABRIC_PRIORITY_SWITCH_INPUT_FLOW 10
		#define FABRIC_PRIORITY_ARP_MISSMATCH_INPUT_FLOW 10
		#define FABRIC_PRIORITY_MISSMATCH_PUSHTAG_FLOW 0
		#define FABRIC_PRIORITY_SWAPTAG_FLOW 20

		#define FABRIC_PRIORITY_FLOATING_EXTERNAL_HOST_SUBNET_FLOW 7
		#define FABRIC_PRIORITY_FLOATING_EXTERNAL_GROUP_SUBNET_FLOW 8
		#define FABRIC_PRIORITY_FLOATING_INTERNAL_SUBNET_FLOW 9
		#define FABRIC_PRIORITY_ARP_FLOW 15
		#define FABRIC_PRIORITY_FLOATING_FLOW 16
		#define FABRIC_PRIORITY_NAT_FLOW 17
		#define FABRIC_PRIORITY_LOADBALANCE_FLOW 18
		#define FABRIC_PRIORITY_DENY_FLOW 19
		#define FABRIC_PRIORITY_FLOATING_LBAAS_FLOW 21

		#define FABRIC_INPUT_TABLE 0
		#define FABRIC_PUSHTAG_TABLE 10
		#define FABRIC_SWAPTAG_TABLE 20
		#define FABRIC_OUTPUT_TABLE 30
	}
}






#endif
