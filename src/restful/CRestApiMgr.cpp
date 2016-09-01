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
*   File Name   : CRestApiHandler.cpp          *
*   Author      : bnc xflu           		*
*   Create Date : 2016-7-21           		*
*   Version     : 1.0           			*
*   Function    : .           				*
*                                                                             *
******************************************************************************/
#include "CRestApiMgr.h"
#include "CRestManager.h"

CRestApiMgr* CRestApiMgr::m_pInstance = 0;

CRestApiMgr::CRestApiMgr()
{
}

CRestApiMgr::~CRestApiMgr()
{
}

CRestApiMgr* CRestApiMgr::getInstance()
{
	if (NULL == m_pInstance) {
		m_pInstance = new CRestApiMgr();
		m_pInstance->init();
	}

	return (m_pInstance);
}

INT4 CRestApiMgr::getRestApiCount()
{
	return (m_restApiList.size());
}

BOOL CRestApiMgr::getRestApiPathList(std::list<std::string> & list)
{
	STL_FOR_LOOP(m_restApiList, iter)
	{
		list.push_back(iter->first.first);
	}

	list.sort();
	list.unique();

	return TRUE;
}

BOOL CRestApiMgr::getRestApiPathList(std::list<restVar> & list)
{
	STL_FOR_LOOP(m_restApiList, iter)
	{
		list.push_back(iter->first);
	}

	return TRUE;
}

BOOL CRestApiMgr::registerRestApi(bnc::restful::http_method method,
								  const std::string & path, restApiHandler handler)
{
	LOG_INFO("register RestApi " + path);

	if (bnc::restful::HTTP_ANY == method)
	{
		method_map methods = CRestManager::getInstance()->getRestDefine()->getMethodList();

		STL_FOR_LOOP(methods, iter)
		{
			m_restApiList.insert(std::make_pair(std::make_pair(path, iter->first), handler));
		}
	}
	else
	{
		m_restApiList.insert(std::make_pair(std::make_pair(path, method), handler));
	}

	return TRUE;
}

BOOL CRestApiMgr::registerDefaultRestApi()
{
	registerRestApi(bnc::restful::HTTP_GET, "/list", api_list);
	registerRestApi(bnc::restful::HTTP_GET, "/list/detail", api_list_detail);
	registerRestApi(bnc::restful::HTTP_ANY, "/echo", api_echo);
	registerRestApi(bnc::restful::HTTP_GET, "/default", api_default);
	registerRestApi(bnc::restful::HTTP_GET, "/json/example", api_json_example);
	registerRestApi(bnc::restful::HTTP_GET, "/switches", api_switches);
	registerRestApi(bnc::restful::HTTP_GET, "/sws", api_switches);
	registerRestApi(bnc::restful::HTTP_GET, "/hosts", api_hosts);

	registerRestApi(bnc::restful::HTTP_GET, "/gn/switchinfo/json", api_get_switch_info);
	return TRUE;
}

BOOL CRestApiMgr::isExist(const restVar & path)
{
	return STL_EXIST(m_restApiList, path);
}

restApiHandler CRestApiMgr::findRestApi(const restVar & var)
{
	restApi::iterator iter = m_restApiList.find(var);

	if (iter != m_restApiList.end())
	{
		return iter->second;
	}

	return NULL;
}

BOOL CRestApiMgr::init()
{
	// clear rest api
	m_restApiList.clear();

	// register default restapi
	registerDefaultRestApi();

	return TRUE;
}

BOOL CRestApiMgr::process(CRestRequest* request, CRestResponse* response)
{
	LOG_IF_RETURN (NULL == request, FALSE,
			"fail to deal with restapi, request is empty.")

	LOG_IF_RETURN (NULL == response, FALSE,
			"fail to deal with restapi, response is empty.")

	LOG_IF_RETURN (request->getPath().empty(), FALSE,
			"fail to deal with restapi, request path is empty.");

	// find RestApi by path
	restApiHandler handler = findRestApi(std::make_pair
			(request->getPath(), request->getMethod()));

	// if fail to find
	if (NULL == handler)
	{
		// set default handler
		handler = findRestApi(std::make_pair("/default", bnc::restful::HTTP_GET));
	}

	LOG_IF_RETURN(NULL == handler, FALSE,
			"fail to find default hander " + request->getPath());

	// call the register function
	(*handler)(request, response);

	return TRUE;
}
