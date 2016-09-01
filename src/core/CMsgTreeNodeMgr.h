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
*   File Name   : CMsgTreeNodeMgr.h           *
*   Author      : bnc zgzhao           *
*   Create Date : 2016-7-5           *
*   Version     : 1.0           *
*   Function    : .           *
*                                                                             *
******************************************************************************/
#ifndef __CMSGTREENODEMGR_H
#define __CMSGTREENODEMGR_H
#include "bnc-type.h"
#include "CRefObj.h"
#include "CSmartPtr.h"
#include "CMsgTreeNode.h"

typedef std::map<CMsgTreeNode*, std::set<std::string>* > CMsgTreeNodeToPath;

/*
 * 消息树节点管理类
 */
class CMsgTreeNodeMgr: public CRefObj
{
public:
    CMsgTreeNodeMgr();
    ~CMsgTreeNodeMgr();

    /*
     * 初始化
     * ret: 成功 TRUE, 失败 FALSE
     */
    BOOL init();

    /*
     * 获取消息树根节点
     * ret: 返回根节点指针
     */
    CSmartPtr<CMsgTreeNode>& getRootNode();

    /*
     * 根据路径名获取对应的节点
     * ret: 返回节点指针
     */
    CSmartPtr<CMsgTreeNode> getMsgTreeNodeByPath(std::string& path);

private:
    /*
     * 生成消息树
     */
    void geneMsgTree();

private:
    CSmartPtr<CMsgTreeNode> m_pRootPtr;                              //消息树根节点
    std::list<CMsgTreeNodeToPath*> m_nodesToPathList;                //保存具体的CMsgTreeNode类与路径的映射关系
    std::map<std::string, UINT4> m_mapPathToType;                    //保存路径与消息类型或者版本类型的对应关系
    std::map<std::string, CSmartPtr<CMsgTreeNode> > m_mapPathToNode; //保存消息树中，路径与节点的映射
};

#endif