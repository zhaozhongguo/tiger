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
*   File Name   : bnc-const.h           *
*   Author      : bnc xflu           	*
*   Create Date : 2016-7-28           	*
*   Version     : 1.0           		*
*   Function    : .           			*
*                                                                             *
******************************************************************************/
#ifndef _CCONST_H
#define _CCONST_H

#include "bnc-type.h"

/*
 * 包含了所使用的的常量
 */

namespace bnc
{
	namespace constant
	{
		/*
		 * restful 使用
		 */
		// http response content默认长度
		const UINT4 response_content_len = 500;

		/*
		 * ARP 常量
		 */
		const UINT2 ARP_CODE_REQUEST = 1;
		const UINT2 ARP_CODE_REPLY = 2;
	}
}

class CBncType
{

};

#endif
