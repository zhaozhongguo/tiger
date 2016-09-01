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
*   File Name   : CRestHttp.h             *
*   Author      : bnc xflu           		*
*   Create Date : 2016-7-22           		*
*   Version     : 1.0           			*
*   Function    : .           				*
*                                                                             *
******************************************************************************/
#ifndef _CRESTHTTP_H
#define _CRESTHTTP_H

#include "CRestDefine.h"

#include <string>
#include <bnc-type.h>
#include "CConst.h"
#include "log.h"

/*
 * HTTP context相关的基类, 提供共有的存取控制
 * the base class of HTTP context related
 */
class CRestHttp
{
public:
	/*
	 * 默认构造函数
	 */
	CRestHttp();

	/*
	 * 默认析构函数
	 */
	~CRestHttp();

	/*
	 * 从字符串构造
	 */
	CRestHttp(std::string raw);

	/*
	 * 获取http版本号
	 * @return: http version
	 */
	const bnc::restful::http_version getVersion() { return m_enHttpVersion; }

	/*
	 * 获取http头部
	 * @return: http header
	 */
	const std::string & getHeader() { return m_strHeader; }

	/*
	 * 获取http body
	 * @return: http body
	 */
	const std::string & getBody() { return m_strBody; }

	/*
	 * 获取http header和body
	 */
	const std::string & getRaw(std::string & raw)
	{ 	return (raw = m_strHeader + "\r\n\r\n" + m_strBody); }

	/*
	 * 获取http头部首行
	 * request: 包括了method/path/version
	 * response: 包括了version/status
	 * @return: http first line
	 */
	const std::string & getHeaderFirstLine() { return m_strFirstLine; }

	/*
	 * 验证http内容有效
	 * @return: TRUE: valid; FALSE: invalid
	 */
	BOOL validate();

	/*
	 * 将字符串格式化为http内容
	 * @param: raw: original string
	 * @return: TRUE:sucess; FALSE: fail
	 */
	BOOL parse(std::string raw);

	/*
	 * 设置header
	 */
	void setHeader(const std::string & header) { m_strHeader = header; }

	/*
	 * 设置body
	 */
	void setBody(const std::string & body) { m_strBody = body; }

private:

	bnc::restful::http_version m_enHttpVersion;		///< http版本
	std::string m_strHeader;						///< http头部
	std::string m_strBody;							///< http body
	std::string m_strFirstLine;						///< http头部首行
};

#endif
