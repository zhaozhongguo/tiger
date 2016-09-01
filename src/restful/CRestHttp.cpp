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
*   File Name   : CRestHttp.cpp      *
*   Author      : bnc xflu           *
*   Create Date : 2016-7-22          *
*   Version     : 1.0                *
*   Function    : .                  *
*                                                                             *
******************************************************************************/
#include "CRestHttp.h"
#include "comm-util.h"

CRestHttp::CRestHttp()
{
}

CRestHttp::~CRestHttp()
{
}

CRestHttp::CRestHttp(std::string raw)
{
	parse(raw);
}

BOOL CRestHttp::validate()
{
	if ((m_strHeader.empty())
		|| (bnc::restful::HTTP_VERSION_OHTER == m_enHttpVersion))
	{
		return FALSE;
	}

	return TRUE;
}

BOOL CRestHttp::parse(std::string raw)
{
	LOG_IF_RETURN (raw.empty(), FALSE, "Fail to parse, string is empty.");

	std::string separator("\r\n\r\n");
	size_t pos = raw.find(separator, 0);

	LOG_IF_RETURN (0 == pos, FALSE, "Fail to find, string is invalid.");

	m_strHeader = raw.substr(0, pos);
	m_strBody = raw.substr(pos + separator.size(), raw.size());

	pos = m_strHeader.find("\n", 0);

	LOG_IF_RETURN (0 == pos, FALSE, "Fail to find first line, string is invalid");

	m_strFirstLine = m_strHeader.substr(0, pos);

	m_enHttpVersion = CRestDefine::getInstance()->getVersionFromStr(m_strFirstLine);

	return TRUE;
}
