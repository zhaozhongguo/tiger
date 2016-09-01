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
*   File Name   : CRestRequest.cpp   *
*   Author      : bnc xflu           *
*   Create Date : 2016-7-21          *
*   Version     : 1.0           	 *
*   Function    : .           		 *
*                                                                             *
******************************************************************************/
#include "CRestResponse.h"

CRestResponse::CRestResponse()
{
}

CRestResponse::~CRestResponse()
{
}

CRestResponse::CRestResponse(bnc::restful::http_version version,
				  	  	  	 bnc::restful::http_status status,
							 const std::string body)
{
	//	example: ("HTTP/1.1 200 OK\r\nContent-Type: application/json"
	//			"\r\nPragma: no-cache\r\n\r\n{123}")

	std::string raw;
	if (createHeader(version, status, body.size(), raw))
	{
		raw.append("\r\n\r\n");
		raw.append(body);
	}

	parse(raw);
}

BOOL CRestResponse::sendResponse(INT4 sockfd)
{
	std::string res;

	write(sockfd, getRaw(res).c_str(), getRaw(res).size());
	close(sockfd);

	return false;
}

BOOL CRestResponse::createHeader(bnc::restful::http_version version,
								 bnc::restful::http_status status,
								 UINT4 content_length,
								 std::string & header)
{
	if ((bnc::restful::HTTP_VERSION_OHTER == version) || (bnc::restful::STATUS_OTHER == status))
	{
		LOG_ERROR("create response failed. invalid version or status.");
		return FALSE;
	}

	m_enHttpStatus = status;

	std::string strVersion;
	std::string strStatus;

	CRestDefine::getInstance()->getStrFromVersion(version, strVersion);
	CRestDefine::getInstance()->getStrFromStatus(status, strStatus);

	if ((strStatus.empty()) || (strVersion.empty()))
	{
		LOG_ERROR("create response failed. status or version string empty");
		return FALSE;
	}

	std::string strContentType("Content-Type: application/json");
	std::string strContentLength("Content-Length: ");

	INT1 strLength[24] = {0};
	content_length = (0 == content_length) ? bnc::constant::response_content_len : content_length;
	sprintf(strLength, "%d", content_length);

	header.append(strVersion);
	header.append(" ");
	header.append(strStatus);
	header.append("\r\n");
	header.append(strContentType);
	header.append("\r\n");
	header.append(strContentLength);
	header.append(strLength);

	return TRUE;
}

BOOL CRestResponse::setResponse(bnc::restful::http_version version,
							    bnc::restful::http_status status,
							    const std::string & body)
{
	std::string header;

	if (createHeader(version, status, body.size(), header))
	{
		setHeader(header);
	}

	setBody(body);

	return TRUE;
}



