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
*   File Name   : comm-util.h           *
*   Author      : bnc zgzhao           *
*   Create Date : 2016-5-25           *
*   Version     : 1.0           *
*   Function    : .           *
*                                                                             *
******************************************************************************/
#ifndef __COMMON_H
#define __COMMON_H
#include "bnc-type.h"

/*
 * 获取cpu总数
 */
INT4 getTotalCpu();

/*
 * 绑定cpu
 */
INT4 setCpuAffinity(INT4 id);

/*
 * 设置无阻塞
 * 为epoll使用
 */
INT4 setNonBlocking(INT4 fd);

/*
 * 增加fd
 * 为epoll使用
 */
BOOL addFd(INT4 epollfd, INT4 fd);

/*
 * UINT8的主机序转网络序
 */
UINT8 htonll(UINT8 n);

/*
 * UINT8的网络序转主机序
 */
UINT8 ntohll(UINT8 n);

/*
 * ip字符串转UINT4
 */
UINT4 ip2number(const INT1* ip);

/*
 * ip UINT4转字符串
 */
INT1* number2ip(INT4 ip_num, INT1* ip);

/*
 * 字符串分割函数
 * @param str: 			原始字符串
 * @param seperator: 	分隔符
 * @param out:			用来存储新字符串的列表
 */
void split(const std::string& str, const std::string& separator, std::list<std::string>& out);

/*
 * 发送消息
 */
INT4 sendMsgOut(INT4 fd, void* data, INT4 len);

/*
 * UINT8转字符串
 */
UINT1 *uint8ToStr(UINT8 value, unsigned char *str);

/*
 * 字符串拼接
 * 将element添加到str中, 并且换行
 * 主要为for_each使用
 */
void strAppend(std::string element, std::string & str);

/*
 * 获得CIDR格式IP地址的子网掩码
 */
UINT4 cidr_to_subnet_mask(UINT4 num);

#endif
