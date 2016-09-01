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
*   File Name   : comm-util.cpp           *
*   Author      : bnc zgzhao           *
*   Create Date : 2016-5-25           *
*   Version     : 1.0           *
*   Function    : .           *
*                                                                             *
******************************************************************************/
#include "comm-util.h"
#include "log.h"

INT4 getTotalCpu()
{
    return sysconf(_SC_NPROCESSORS_CONF);
}

INT4 setCpuAffinity(INT4 id)
{
    INT4 total = getTotalCpu();
    if (total < id)
    {
        LOG_ERROR("invalid cpu id");
        return -1;
    }
    cpu_set_t cpu_set;
    CPU_ZERO(&cpu_set);
    CPU_SET(id, &cpu_set);
    return pthread_setaffinity_np(pthread_self(), sizeof(cpu_set_t), &cpu_set);
}

INT4 setNonBlocking(INT4 fd)
{
    INT4 old_flag = fcntl(fd, F_GETFL);
    INT4 new_flag = old_flag | O_NONBLOCK;
    fcntl(fd, F_SETFL, new_flag);
    return old_flag;
}

BOOL addFd(INT4 epollfd, INT4 fd)
{
    setNonBlocking(fd);
    epoll_event event;
    event.data.fd = fd;
    event.events = EPOLLIN | EPOLLET;
    if (epoll_ctl(epollfd, EPOLL_CTL_ADD, fd, &event) < 0)
    {
        LOG_ERROR("epoll_ctl failure");
        return FALSE;
    }

    return TRUE;
}

UINT8 htonll(UINT8 n)
{
    return htonl(1) == 1 ? n : ((UINT8) htonl(n) << 32) | htonl(n >> 32);
}

UINT8 ntohll(UINT8 n)
{
    return htonl(1) == 1 ? n : ((UINT8) ntohl(n) << 32) | ntohl(n >> 32);
}

UINT4 ip2number(const INT1* ip)
{
   return inet_addr(ip);
}

INT1* number2ip(INT4 ip_num, INT1* ip)
{
   strcpy(ip, inet_ntoa(*(struct in_addr*)&ip_num));
   return ip;
}


void split(const std::string& str, const std::string& separator, std::list<std::string>& out)
{
    if (str.empty())
    {
        return;
    }
    
    std::size_t pos = 0;
    std::size_t cur = 0;
    std::size_t len = str.size();
    while (std::string::npos != (cur = str.find(separator, pos)))
    {
        out.push_back(str.substr(pos, cur - pos));
        pos = cur + 1;
    }

    out.push_back(str.substr(pos, len - pos));
}

INT4 sendMsgOut(INT4 fd, void* data, INT4 len)
{
    INT4 ret = write(fd, data, len);
    INT4 tryTimes = 0;
//    while ((ret = write(fd, data, len)) < 0)
//    {
//        if (tryTimes < 5)
//        {
//            usleep(200000);
//        }
//        else
//        {
//            break;
//        }
//        ++tryTimes;
//    }
    return ret;
}


UINT1 *uint8ToStr(UINT8 value, unsigned char *str)
{
    str[0]= (unsigned char)((value>>56) & 0x00000000000000ff);
    str[1]= (unsigned char)((value>>48) & 0x00000000000000ff);
    str[2]= (unsigned char)((value>>40) & 0x00000000000000ff);
    str[3]= (unsigned char)((value>>32) & 0x00000000000000ff);
    str[4]= (unsigned char)((value>>24) & 0x00000000000000ff);
    str[5]= (unsigned char)((value>>16) & 0x00000000000000ff);
    str[6]= (unsigned char)((value>> 8) & 0x00000000000000ff);
    str[7]= (unsigned char)((value>> 0) & 0x00000000000000ff);

    return str;
}

void strAppend(std::string element, std::string & str)
{
	str.append(element + "\n");
}

UINT4 cidr_to_subnet_mask(UINT4 num)
{
	UINT4 mask = 0;

	if (32 >= num) {
	    int bits = sizeof(UINT4) * 8;

	    mask = ~0;
	    bits -= num;
	    mask <<= bits;
	}

    return htonl(mask);
}
