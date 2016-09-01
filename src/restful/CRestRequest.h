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
*   File Name   : CRestRequest.h	*
*   Author      : bnc xflu          *
*   Create Date : 2016-7-21         *
*   Version     : 1.0           	*
*   Function    : .           		*
*                                                                             *
******************************************************************************/
#ifndef _CRESTREQUEST_H
#define _CRESTREQUEST_H

#include "CRestHttp.h"
#include "CRestResponse.h"

/*
 * HTTP Request相关类
 */
class CRestRequest : public CRestHttp
{
public:
	/*
	 * 使用string构造函数
	 */
	CRestRequest(std::string & raw);

	/*
	 * 默认析构函数
	 */
	~CRestRequest();

	/*
	 * 获取method
	 */
	const bnc::restful::http_method getMethod()	{ return m_enHttpMethod; }

	/*
	 * 获取path
	 */
	const std::string & getPath() {	return m_strPath; }

private:
	/*
	 * 默认构造函数
	 */
	CRestRequest();

	bnc::restful::http_method m_enHttpMethod;		///< http method
	std::string m_strPath;							///< http uri path

};

#endif
