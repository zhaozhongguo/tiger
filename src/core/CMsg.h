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
*   File Name   : CMsg.h           *
*   Author      : bnc zgzhao           *
*   Create Date : 2016-5-25           *
*   Version     : 1.0           *
*   Function    : .           *
*                                                                             *
******************************************************************************/
#ifndef __CMSG_H
#define __CMSG_H
#include "bnc-type.h"
#include "openflow-common.h"
#include "CRefObj.h"
#include "bnc-param.h"

/*
 * openflow消息封装类
 */
class CMsg: public CRefObj
{
public:
    CMsg(UINT1* data, INT4 len);
    ~CMsg();

    /*
     * 获取socket连接描述符
     * ret: 返回socket连接描述符
     */
    INT4 getSockfd();

    /*
     * 设置socket连接描述符
     * param sockfd: 文件描述符
     */
    void setSockfd(INT4 sockfd);

    /*
     * 获取消息内容指针
     * ret: 返回消息内容指针
     */
    UINT1* getData();

    /*
     * 获取消息长度
     * ret: 返回消息长度
     */
    INT4 length();

    /*
     * 获取协议版本
     * ret: 返回协议版本
     */
    UINT1 getVersion();

    /*
     * 设置协议版本
     * param version: 协议版本
     */
    void setVersion(UINT1 version);

    /*
     * 获取消息类型
     * ret: 返回消息类型
     */
    UINT1 getOfType();

    /*
     * 设置消息类型
     * param type: 消息类型
     */
    void setOfType(UINT1 type);

    /*
     * 获取消息状态
     * ret: 返回消息状态OF_MSG_STATE
     */
    INT4 getState();

    /*
     * 设置消息状态
     * param state: 消息状态OF_MSG_STATE
     */
    void setState(INT4 state);

    /*
     * 获取packet in
     * ret: 返回packet in结构指针
     */
    packet_in_info_t* getPacketIn();

    /*
     * 检测openflow消息是否合法
     * ret: 合法 GN_OK，不合法 GN_ERR
     */
    INT4 check();
    
private:
    UINT1* m_strData;                //消息码流
    packet_in_info_t* m_pPacketIn;   //packet_in消息指针,只有为PACKET_IN消息时有效
    INT4 m_iLen;                     //长度
    INT4 m_iSockfd;                  //socket
    UINT1 m_uiVersion;               //协议版本
    UINT1 m_uiofType;                //消息类型
    INT4 m_iState;                   //消息处理状态 OF_MSG_STATE
};

#endif