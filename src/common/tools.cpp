/**
* @file  tools.cpp
* @brief  日志
* @author  zhaozhongguo
* @date  2016-9-14 
* @version  1.0
*/
#include <unistd.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include "tools.h"
#include "log.h"

namespace common
{
    int setNonBlocking(int fd)
    {
        int old_flag = fcntl(fd, F_GETFL);
        int new_flag = old_flag | O_NONBLOCK;
        fcntl(fd, F_SETFL, new_flag);
        return old_flag;
    }


    uint32_t ip2Int(const char* ip)
    {
        return ntohl(inet_addr(ip));
    }


    char* int2Ip(uint32_t ipNum, char* ip)
    {
        struct in_addr in;
        in.s_addr = htonl(ipNum);
        strncpy(ip, (char*)inet_ntoa(in), IPV4_STR_LEN);
        return ip;
    }
}
