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
*   File Name   : CEtherMsgTreeNode.cpp           *
*   Author      : bnc zgzhao           *
*   Create Date : 2016-7-5           *
*   Version     : 1.0           *
*   Function    : .           *
*                                                                             *
******************************************************************************/
#include "CEtherMsgTreeNode.h"
#include "bnc-inet.h"
#include "log.h"
#include "CMsg.h"
#include "COfMsgUtil.h"
#include "comm-util.h"

CEtherMsgTreeNode::CEtherMsgTreeNode()
{
}

CEtherMsgTreeNode::~CEtherMsgTreeNode()
{
}

BOOL CEtherMsgTreeNode::checkMsg(CSmartPtr<CMsg>& msgPtr)
{
    if (msgPtr.isNull())
    {
        return FALSE;
    }

    UINT1* ofMsg = msgPtr->getData();
    packet_in_info_t* packetIn = msgPtr->getPacketIn();
    memset(packetIn, 0, sizeof(packet_in_info_t));
    if (COfMsgUtil::convertMsgToPacketIn(ofMsg, *packetIn))
    {
        ether_t* ether_header = (ether_t*)packetIn->data;
        if (getMsgType() == htons(ether_header->proto))
        {
            return TRUE;
        }
    }

    return FALSE;
}

CSmartPtr<CMsgTreeNode> CEtherMsgTreeNode::clone()
{
    return CSmartPtr<CMsgTreeNode>(new CEtherMsgTreeNode());
}


std::string& CEtherMsgTreeNode::geneSubQueueKey(CSmartPtr<CMsg>& msgPtr)
{
    if (m_strSubQueueKey.empty())
    {
        return DEFAULT_NODE_PATH;
    }

    packet_in_info_t* packetIn = msgPtr->getPacketIn();
    ip_t* p_ip = (ip_t*)(packetIn->data);

    INT1 buf[32] = {0};
    INT4 pos = 0;
    std::list<std::string> strList;
    split(m_strSubQueueKey, ",", strList);
    std::list<std::string>::iterator iter = strList.begin();
    for (; iter != strList.end(); iter++)
    {
        if ("src_ip" == *iter)
        {
            sprintf(buf + pos, "%u", p_ip->src);
            pos += sizeof(p_ip->src);
        }
        else if ("dst_ip" == *iter)
        {
            sprintf(buf + pos, "%u", p_ip->dest);
            pos += sizeof(p_ip->dest);
        }
        else if ("src_mac" == *iter)
        {
            strncpy(buf + pos, (INT1*)p_ip->eth_head.src, MAC_LEN);
            pos += MAC_LEN;
        }
        else if ("dst_mac" == *iter)
        {
            strncpy(buf + pos, (INT1*)p_ip->eth_head.dest, MAC_LEN);
            pos += MAC_LEN;
        }
    }

    //清空缓存
    m_strKeyBuffer.clear();
    m_strKeyBuffer.assign(buf);
    LOG_DEBUG_FMT("gene sub queue key : %s", m_strKeyBuffer.c_str());

    return m_strKeyBuffer;
}