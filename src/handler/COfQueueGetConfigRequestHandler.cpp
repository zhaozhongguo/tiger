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
*   File Name   : COfQueueGetConfigRequestHandler.cpp           *
*   Author      : bnc zgzhao           *
*   Create Date : 2016-7-6           *
*   Version     : 1.0           *
*   Function    : .           *
*                                                                             *
******************************************************************************/
#include "COfQueueGetConfigRequestHandler.h"
#include "log.h"

COfQueueGetConfigRequestHandler::COfQueueGetConfigRequestHandler()
{
}

COfQueueGetConfigRequestHandler::~COfQueueGetConfigRequestHandler()
{
}

void COfQueueGetConfigRequestHandler::handle(CSmartPtr<CMsgQueue>& queue)
{
    LOG_INFO_FMT("%s recv new msg ..., queue size = %lu", toString().c_str(), queue->size());
    queue->clear();
}

std::string COfQueueGetConfigRequestHandler::toString()
{
    return "COfQueueGetConfigRequestHandler";
}

CMsgHandler* COfQueueGetConfigRequestHandler::clone()
{
    CMsgHandler* instance = new COfQueueGetConfigRequestHandler();
    *instance = *this;
    return instance;
}