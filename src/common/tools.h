/**
* @file  tools.h
* @brief  日志
* @author  zhaozhongguo
* @date  2016-9-14 
* @version  1.0
*/
#ifndef __TOOLS_H
#define __TOOLS_H
#include "type-def.h"

namespace common
{
    /** 
    * @brief 设置非阻塞模式
    * @param fd: 文件描述符
    * @return 返回老的file status flags
    */
    int setNonBlocking(int fd);

    /** 
    * @brief 点分十进制ip地址转整数
    * @param ip: ip地址
    * @return 返回整数值
    */
    uint32_t ip2Int(const char* ip);

    /** 
    * @brief 整数转点分十进制ip地址
    * @param ipNum: 整数ip
    * @param ip: 输出参数，ip地址
    * @return 返回ip地址
    */
    char* int2Ip(uint32_t ipNum, char* ip);
}


#endif
