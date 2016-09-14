/**
* @file  type-def.h
* @brief  公共类型定义
* @author  zhaozhongguo
* @date  2016-9-12 
* @version  1.0
*/
#ifndef __TYPE_DEF_H
#define __TYPE_DEF_H

#include <stdint.h>

#define IPV4_STR_LEN 16

//socket事件类型
enum ENUM_SOCKET_EVENT
{
    SOCKET_CONNECT_EVENT,
    SOCKET_READ_EVENT,
    SOCKET_WRITE_EVENT,
};
    
typedef struct Connection
{
    int sockfd;
    uint32_t ip;
    uint16_t port;
} Connection_t;


#endif