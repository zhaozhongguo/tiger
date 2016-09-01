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
*   File Name   : dcfabric.cpp           *
*   Author      : bnc zgzhao           *
*   Create Date : 2016-5-25           *
*   Version     : 1.0           *
*   Function    : .           *
*                                                                             *
******************************************************************************/
#include "dcfabric.h"
#include "log.h"
#include "CTCPServer.h"
#include "CHttpRecvWorker.h"
#include "COfpRecvWorker.h"
#include "CTopoMgr.h"
#include "CServer.h"

void startServer()
{
    //屏蔽PIPE信号
    struct sigaction action;
    action.sa_handler = SIG_IGN;
    sigemptyset(&action.sa_mask);
    action.sa_flags = 0;
    if (sigaction(SIGPIPE, &action, NULL) < 0)
    {
        LOG_ERROR("sigaction failure");
        return;
    }

    const INT1* pMaxFiles = CServer::getInstance()->getConf()->getConfig("controller", "max_open_files");
    INT4 maxFiles = (NULL == pMaxFiles) ? 20000 : atol(pMaxFiles);
    //设置进程允许打开最大文件数
    struct rlimit rt;
    rt.rlim_max = rt.rlim_cur = maxFiles;
    if (setrlimit(RLIMIT_NOFILE, &rt) < 0) 
    {
        LOG_ERROR("setrlimit error");
        return;
    }

    //初始化消息树
    if (!CServer::getInstance()->getMsgTreeNodeMgr()->init())
    {
        LOG_ERROR("msg tree node mgr init failure");
        return;
    }

    //初始化handler
    if (!CServer::getInstance()->getHandlerMgr()->init())
    {
        LOG_ERROR("handler mgr init failure");
        return;
    }
    CServer::getInstance()->getHandlerMgr()->start();

    //读取of服务端口
    const INT1* pOfport = CServer::getInstance()->getConf()->getConfig("controller", "openflow_server_port");
    UINT4 ofPort = (NULL == pOfport) ? 6633 : atoll(pOfport);

    //读取接收线程个数
    const INT1* pRecvThreadNum = CServer::getInstance()->getConf()->getConfig("controller", "recv_thread_num");
    UINT4 recvThreadNum = (pRecvThreadNum == NULL) ? 10 : atol(pRecvThreadNum);

    //启动of server
    CSmartPtr<CRecvWorker> ptrOfRecvWorker(new COfpRecvWorker());
    CTCPServer tcpServer(0, ofPort, recvThreadNum, maxFiles, ptrOfRecvWorker);
    if (!tcpServer.init())
    {
        LOG_ERROR("ofp server init failure");
        return;  
    }

    //读取http服务端口
    const INT1* pHttpport = CServer::getInstance()->getConf()->getConfig("restful", "http_server_port");
    UINT4 httpPort = (NULL == pHttpport) ? 8081 : atoll(pHttpport);

    //启动http server
    CSmartPtr<CRecvWorker> ptrHttpRecvWorker(new CHttpRecvWorker());
    CTCPServer httpServer(0, httpPort, recvThreadNum, maxFiles, ptrHttpRecvWorker);
    if (!httpServer.init())
    {
        LOG_ERROR("http server init failure");
        return;  
    }

    LOG_INFO("dcfabric server start success");
    while (true)
    {
        sleep(30);
        CServer::getInstance()->getTopoMgr()->printPaths();
    }
}




int main()
{
    //初始化日志系统
    initLogger();

    //加载配置
    CServer::getInstance()->getConf()->loadConf();

    //启动topo管理模块
    if (!CServer::getInstance()->getTopoMgr()->init())
    {
        LOG_ERROR("topo module init failure");
        return 0;
    }

    //启动服务
    startServer();
    return 0;
}
