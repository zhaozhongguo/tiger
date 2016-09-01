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
*   File Name   : CHttpRecvWorker.cpp           *
*   Author      : bnc zgzhao           *
*   Create Date : 2016-7-21           *
*   Version     : 1.0           *
*   Function    : .           *
*                                                                             *
******************************************************************************/
#include "CHttpRecvWorker.h"
#include "CBuffer.h"
#include "CMsg.h"
#include "CRestManager.h"

CHttpRecvWorker::CHttpRecvWorker()
{
}

CHttpRecvWorker::~CHttpRecvWorker()
{
}

void CHttpRecvWorker::processMsgIn(INT4 sockfd, CSmartPtr<CBuffer> inMsg)
{
    std::string raw = (char*)inMsg->getData();

    // 创建request和response
	CRestRequest* request = new CRestRequest(raw);
	CRestResponse* response = new CRestResponse();

	// 通过restAPI提供的接口来处理
	CRestManager::getInstance()->getRestApiMgr()->process(request, response);

	// 发送response
	response->sendResponse(sockfd);

	// 清除buffer
	inMsg.getPtr()->clear();

	// 删除指针
	delete request;
	delete response;
}

CRecvWorker* CHttpRecvWorker::clone()
{
    return new CHttpRecvWorker();
}
