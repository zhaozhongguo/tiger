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
*   File Name   : CRestApiHandler.h          *
*   Author      : bnc xflu           		*
*   Create Date : 2016-7-22           		*
*   Version     : 1.0           			*
*   Function    : .           				*
*                                                                             *
******************************************************************************/
#ifndef _CRESTAPI_H
#define _CRESTAPI_H

#include "bnc-type.h"
#include "CRestRequest.h"
#include "CRestResponse.h"

/*
 * 定义了所有使用的RestAPI
 */
// use function pointer to register
typedef void(*restApiHandler)(CRestRequest* request, CRestResponse* response);


// RestApi实例, 使用了函数指针, 所以需要定义为普通函数, 而不是成员函数
// *此处后续会修改, 可能使用boost::function或者std::function, 先测试使用

/*
 * 测试用
 * 会返回和请求相同的Body
 */
void api_echo(CRestRequest* request, CRestResponse* response);

/*
 * 测试用
 * 会列出所有已经注册的Api
 */
void api_list(CRestRequest* request, CRestResponse* response);

/*
 * 测试用
 * 会列出所有已经注册的Api的详细信息
 */
void api_list_detail(CRestRequest* request, CRestResponse* response);

/*
 * 测试用
 * 默认处理函数, 如果请求的path不存在, 会调用这个函数
 */
void api_default(CRestRequest* request, CRestResponse* response);

/*
 * 测试用
 * 一个简单的json数据dom的转换和使用
 */
void api_json_example(CRestRequest* request, CRestResponse* response);

/*
 * 列出所有的交换机信息
 */
void api_switches(CRestRequest* request, CRestResponse* response);

/*
 * 列出所有的主机信息
 */
void api_hosts(CRestRequest* request, CRestResponse* response);

/*
 * 取得所有的交换机信息
 * 兼容原有api
 */
void api_get_switch_info(CRestRequest* request, CRestResponse* response);

#endif
