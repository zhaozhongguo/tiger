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
*   File Name   : CRecvWorker.h           *
*   Author      : bnc zgzhao           *
*   Create Date : 2016-5-25           *
*   Version     : 1.0           *
*   Function    : .           *
*                                                                             *
******************************************************************************/
#ifndef __CRECVWORKER__H
#define __CRECVWORKER__H
#include "bnc-type.h"
#include "CSmartPtr.h"
#include "CRefObj.h"

class CBuffer;

/**
 * worker类
 *     负责接收并封装消息入队列
 */
class CRecvWorker: public CRefObj
{
public:
    CRecvWorker();
    virtual ~CRecvWorker();

    virtual void processMsgIn(INT4 sockfd, CSmartPtr<CBuffer> inMsg);
    virtual void processConnClosed(INT4 sockfd);
    virtual CRecvWorker* clone();
    virtual BOOL addConnectFd(INT4 fd, UINT4 ip, UINT2 port);

    BOOL init(
        INT4 maxEpollFiles,
        INT4 recvBufLen,
        std::map<INT4, CSmartPtr<CBuffer> >* recvBuf);
    

    static void* work(void* param);

protected:
    INT4 getEpollFd();
    INT4 getMaxEpollFiles();
    void msgIn(INT4 sockfd);

private:
    INT4 m_epollFd;
    pthread_t m_threadId;
    INT4 m_iMaxEpollFiles;
    INT4 m_iRecvBufLen;
    UINT1* m_strRecvBuf;
    std::map<INT4, CSmartPtr<CBuffer> >* m_mapRecvBuf;
};




#endif
