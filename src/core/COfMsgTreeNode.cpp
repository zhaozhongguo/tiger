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
*   File Name   : COfMsgTreeNode.cpp           *
*   Author      : bnc zgzhao           *
*   Create Date : 2016-7-5           *
*   Version     : 1.0           *
*   Function    : .           *
*                                                                             *
******************************************************************************/
#include "COfMsgTreeNode.h"
#include "CServer.h"
#include "log.h"
#include "CMsg.h"
#include "bnc-inet.h"

COfMsgTreeNode::COfMsgTreeNode()
{
}

COfMsgTreeNode::~COfMsgTreeNode()
{
}

BOOL COfMsgTreeNode::checkMsg(CSmartPtr<CMsg>& msgPtr)
{
    if (msgPtr.isNull())
    {
        return FALSE;
    }

    //of消息类型检查
    UINT1* ofMsg = msgPtr->getData();
    struct ofp_header* header = (struct ofp_header*)ofMsg;
    if (header->type != getMsgType())
    {
        return FALSE;
    }

    return TRUE;
}

CSmartPtr<CMsgTreeNode> COfMsgTreeNode::clone()
{
    return CSmartPtr<CMsgTreeNode>(new COfMsgTreeNode());
}