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
*   File Name   : CTCPServer.h           *
*   Author      : bnc zgzhao           *
*   Create Date : 2016-7-21           *
*   Version     : 1.0           *
*   Function    : .           *
*                                                                             *
******************************************************************************/
#ifndef __CTCPSERVER__H
#define __CTCPSERVER__H
#include "bnc-type.h"
#include "CSmartPtr.h"
#include "CRefObj.h"

class CBuffer;
class CRecvWorker;

/**
 * 连接管理类
 *     负责管理与交换机的连接，接受来自交换机的消息
 */
class CTCPServer: public CRefObj
{
public:
    CTCPServer(
        UINT4 ip, 
        UINT4 port, 
        INT4 threadNum, 
        INT4 maxOpenFiles,
        CSmartPtr<CRecvWorker> ptrRecvWorker);

    ~CTCPServer();

public:
    /*
     * 初始化
     */
    BOOL init();

    /*
     * 启动server
     */
    friend void* startServer(void* param);

private:
    BOOL listen();
    void process();
    void accept();

private:
    //连接ip
    UINT4 m_uiIP;

    //监听端口
    UINT4 m_uiPort;

    //最大文件打开个数
    INT4 m_iMaxEpollFiles;

    //接收线程个数
    INT4 m_iRecvThreadSize;

    //监听socket文件描述符
    INT4 m_iListenfd;

    //接收缓冲区长度
    INT4 m_iRecvBufLen;

    //接收worker模板
    CSmartPtr<CRecvWorker> m_ptrRecvWorker;

    //消息接收worker列表
    std::vector<CSmartPtr<CRecvWorker> > m_workerVec;

    //每个worker对应的接收缓冲区
    std::map<INT4, CSmartPtr<CBuffer> > m_mapRecvBuf;

    //线程id
    pthread_t m_threadId;
};




#endif
