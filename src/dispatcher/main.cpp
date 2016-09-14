/**
* @file  main
* @brief  main
* @author  zhaozhongguo
* @date  2016-9-14
* @version  1.0
*/

#include <unistd.h>
#include "type-def.h"
#include "tcp-server.h"
#include "tools.h"
#include "log.h"

void* connectionEventCallBack(void* param)
{
    char buf[IPV4_STR_LEN] = {0};
    Connection_t* conn = (Connection_t*)param;
    LOG_INFO_FMT("new connection: ip=%s, port=%d", common::int2Ip(conn->ip, buf), conn->port);

    return NULL;
}

void* readEventCallBack(void* param)
{
    LOG_INFO_FMT("read event: sockfd=%d", *((int*)param));
    return NULL;
}

void* writeEventCallBack(void* param)
{
    LOG_INFO_FMT("read event: sockfd=%d", *((int*)param));
    return NULL;
}

int main()
{
    //后台运行
    daemon(1, 0);

    //日志初始化
    initLogger();

    //起tcp监听服务
    dispatcher::CTcpServer tcpServer(0, 6633, 10);
    tcpServer.start();



    while (true)
    {
        sleep(10);
    }
    return 0;
}