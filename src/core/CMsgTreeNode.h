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
*   File Name   : CMsgTreeNode.h           *
*   Author      : bnc zgzhao           *
*   Create Date : 2016-7-5           *
*   Version     : 1.0           *
*   Function    : .           *
*                                                                             *
******************************************************************************/
#ifndef __CMSGTREENODE_H
#define __CMSGTREENODE_H
#include "bnc-type.h"
#include "CRefObj.h"
#include "CSmartPtr.h"
#include "CList.h"
#include "CMap.h"
#include "CMutex.h"
#include "CLFQueue.h"

class CMsg;

// typedef CList<CSmartPtr<CMsg> > CMsgQueue;
typedef CLFQueue<CSmartPtr<CMsg> > CMsgQueue;

/*
 * 消息存储树节点类
 */
class CMsgTreeNode: public CRefObj
{
public:
    CMsgTreeNode();
    virtual ~CMsgTreeNode();

    /*
     * 检测消息是否合法
     * ret: 合法 TRUE，不合法 FALSE
     */
    virtual BOOL checkMsg(CSmartPtr<CMsg>& msgPtr);

    /*
     * 存储消息
     * param: 消息指针
     * ret: 返回存储路径
     */
    virtual std::string& putMsg(CSmartPtr<CMsg>& msgPtr);

    /*
     * 生成子队列关键字
     * param: 消息指针
     * ret: 返回队列关键字
     */
    virtual std::string& geneSubQueueKey(CSmartPtr<CMsg>& msgPtr);

    /*
     * 克隆方法
     * ret: 返回对象克隆实例
     */
    virtual CSmartPtr<CMsgTreeNode> clone();

    /*
     * 获取消息
     * ret: 返回消息子队列
     */
    CSmartPtr<CMsgQueue> getMsgQueue();

    /*
     * 存储消息都子节点
     * param: 消息指针
     * ret: 返回存储路径
     */
    std::string& putMsgToChildNodes(CSmartPtr<CMsg>& msgPtr);

    /*
     * 获取子队列关键字，根据此关键字划分子队列
     * param: 子队列关键字
     */
    void setSubQueueKey(std::string& key);

    /*
     * 获取子队列关键字
     * ret: 返回子队列关键字
     */
    std::string& getSubQueueKey();

    /*
     * 设置路径
     * param: 路径
     */
    void setNodePath(std::string& nodePath);

    /*
     * 获取路径
     * ret: 返回路径
     */
    std::string& getNodePath();

    /*
     * 设置消息类型
     * param: 消息类型
     */
    void setMsgType(UINT4 msgType);

    /*
     * 获取消息类型
     * ret: 返回消息类型
     */
    UINT4 getMsgType();

    /*
     * 添加子节点
     * param: 子节点
     */
    void addChildNode(CSmartPtr<CMsgTreeNode>& node);

    /*
     * 根据路径获取子节点
     * param: 路径
     * ret: 子节点指针
     */
    CSmartPtr<CMsgTreeNode> findChildNode(std::string path);

public:
    static std::string DEFAULT_NODE_PATH;

protected:
    CMutex m_oMutex;
    std::string m_strSubQueueKey;                                        //子队列划分关键字
    std::string m_strKeyBuffer;                                          //关键字缓冲区

private:
    std::string m_strNodePath;                                           //节点路径
    UINT4 m_uiMsgType;                                                   //消息类型
    std::list<CSmartPtr<CMsgTreeNode> > m_childNodeList;                 //子节点列表
    CMap<std::string,  CSmartPtr<CMsgQueue> > m_mapMsgQueue;             //消息队列集
    UINT4 m_uiMsgQueuePos;                                               //消息队列集的当前位置
};

#endif