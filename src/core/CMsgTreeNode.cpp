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
*   File Name   : CMsgTreeNode.cpp           *
*   Author      : bnc zgzhao           *
*   Create Date : 2016-7-5           *
*   Version     : 1.0           *
*   Function    : .           *
*                                                                             *
******************************************************************************/
#include "CMsgTreeNode.h"
#include "CServer.h"
#include "log.h"
#include "bnc-inet.h"

#define MSG_QUEUE_BUFFER_LEN 32

std::string CMsgTreeNode::DEFAULT_NODE_PATH = "";

CMsgTreeNode::CMsgTreeNode():
    m_uiMsgType(0),
    m_uiMsgQueuePos(0)
{
    m_strKeyBuffer.resize(MSG_QUEUE_BUFFER_LEN);
    m_strKeyBuffer.clear();
}

CMsgTreeNode::~CMsgTreeNode()
{
}

BOOL CMsgTreeNode::checkMsg(CSmartPtr<CMsg>& msgPtr)
{
    if (msgPtr.isNull())
    {
        return FALSE;
    }

    //of消息类型检查
    UINT1* ofMsg = msgPtr->getData();
    struct ofp_header* header = (struct ofp_header*)ofMsg;
    if (header->version != m_uiMsgType)
    {
        return FALSE;
    }

    return TRUE;
}

std::string& CMsgTreeNode::putMsg(CSmartPtr<CMsg>& msgPtr)
{
    if (!checkMsg(msgPtr))
    {
        LOG_TRACE_FMT("invalid msg, discard it, path : %s", m_strNodePath.c_str());
        return DEFAULT_NODE_PATH;
    }

    //若为叶节点直接保存消息到队列
    if (m_childNodeList.empty())
    {
        m_oMutex.lock();
        LOG_TRACE_FMT("put msg into queue, path : %s", m_strNodePath.c_str());
        std::string& queueKey = geneSubQueueKey(msgPtr);
        CMap<std::string,  CSmartPtr<CMsgQueue> >::iterator iter = m_mapMsgQueue.find(queueKey);
        if (iter != m_mapMsgQueue.end())
        {
            iter->second->push_back(msgPtr);
        }
        else
        {
            CSmartPtr<CMsgQueue> ll(new CMsgQueue());
            ll->push_back(msgPtr);
            m_mapMsgQueue.insert(std::pair<std::string,  CSmartPtr<CMsgQueue> >(queueKey, ll));
        }
        LOG_TRACE_FMT("success to put msg into queue, path : %s", m_strNodePath.c_str());
        m_oMutex.unlock();
        return m_strNodePath;
    }
    //若为非叶节点传递消息到子节点
    else
    {
        return putMsgToChildNodes(msgPtr);
    }
}


std::string& CMsgTreeNode::putMsgToChildNodes(CSmartPtr<CMsg>& msgPtr)
{
    LOG_TRACE_FMT("pass msg to child nodes, path : %s", m_strNodePath.c_str());
    std::list<CSmartPtr<CMsgTreeNode> >::iterator iter = m_childNodeList.begin();
    for (; iter != m_childNodeList.end(); iter++)
    {
        std::string& ret = (*iter)->putMsg(msgPtr);
        if (!ret.empty())
        {
            return ret;
        }
    }

    return DEFAULT_NODE_PATH;
}


CSmartPtr<CMsgTreeNode> CMsgTreeNode::clone()
{
    return CSmartPtr<CMsgTreeNode>(new CMsgTreeNode());
}

CSmartPtr<CMsgQueue> CMsgTreeNode::getMsgQueue()
{
    CSmartPtr<CMsgQueue> ret(NULL);

    m_oMutex.lock();
    UINT4 size = m_mapMsgQueue.size();
    if (size <= 0)
    {
        m_oMutex.unlock();
        return ret;
    }

    m_uiMsgQueuePos %= size;
    CMap<std::string,  CSmartPtr<CMsgQueue> >::iterator iter = m_mapMsgQueue.begin();
    advance(iter, m_uiMsgQueuePos++);
    //从上一次的位置往后开始寻找一个有效的队列
    for (UINT4 i = 0; i < size; i++)
    {
        if (iter->second.isNull() || iter->second->empty())
        {
            //删除空队列
            CMap<std::string,  CSmartPtr<CMsgQueue> >::iterator temp = iter++;
            m_mapMsgQueue.erase(temp);
        }
        else if (!iter->second->isBusy())
        {
            ret = iter->second;
            iter->second->setBusy(TRUE);
            break;
        }
        else
        {
            iter++;
        }

        //如已经到end，则从头开始
        if (iter == m_mapMsgQueue.end())
        {
            iter = m_mapMsgQueue.begin();
        }
    }
    m_oMutex.unlock();

    return ret;
}

void CMsgTreeNode::setNodePath(std::string& nodePath)
{
    m_strNodePath = nodePath;
}

std::string& CMsgTreeNode::getNodePath()
{
    return m_strNodePath;
}

void CMsgTreeNode::setMsgType(UINT4 msgType)
{
    m_uiMsgType = msgType;
}

UINT4 CMsgTreeNode::getMsgType()
{
    return m_uiMsgType;
}

void CMsgTreeNode::addChildNode(CSmartPtr<CMsgTreeNode>& node)
{
    m_childNodeList.push_back(node);
}

CSmartPtr<CMsgTreeNode> CMsgTreeNode::findChildNode(std::string path)
{
    CSmartPtr<CMsgTreeNode> ret(NULL);
    std::list<CSmartPtr<CMsgTreeNode> >::iterator iter = m_childNodeList.begin();
    for (; iter != m_childNodeList.end(); iter++)
    {
        if ((*iter)->getNodePath() == path)
        {
            return *iter;
        }
    }

    return ret;
}

std::string& CMsgTreeNode::geneSubQueueKey(CSmartPtr<CMsg>& msgPtr)
{
    return DEFAULT_NODE_PATH;
}

void CMsgTreeNode::setSubQueueKey(std::string& key)
{
    m_strSubQueueKey = key;
}

std::string& CMsgTreeNode::getSubQueueKey()
{
    return m_strSubQueueKey;
}