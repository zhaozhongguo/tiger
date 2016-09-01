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
*   File Name   : CBuffer.h           *
*   Author      : bnc zgzhao           *
*   Create Date : 2016-5-25           *
*   Version     : 1.0           *
*   Function    : .           *
*                                                                             *
******************************************************************************/
#ifndef __CBUFFER_H
#define __CBUFFER_H
#include "bnc-type.h"
#include "CSmartPtr.h"
#include "CRefObj.h"
#include "CMutex.h"

class CMsg;

extern INT4 RECV_BUF_SIZE;

/*
 * 缓冲区类
 */
class CBuffer: public CRefObj
{
public:
    CBuffer();
    CBuffer(INT4 size);
    ~CBuffer();

    /*
     * 向缓冲区写入数据
     * param data: 指向数据的指针
     * param len: 数据长度
     * ret: 返回指向缓冲区数据首地址的指针
     */
    UINT1* write(UINT1* data, INT4 len);

    /*
     * 获取缓冲区数据长度
     * ret: 返回缓冲区数据长度
     */
    INT4 size();

    /*
     * 获取当前位置
     * ret: 返回当前位置
     */
    INT4 current();

    /*
     * 获取尾部位置
     * ret: 返回尾部位置
     */
    INT4 tail();

    /*
     * 获取缓冲区指针
     * ret: 返回缓冲区指针
     */
    UINT1* getData();

    /*
     * 获取CMsg消息指针：封装缓冲区数据为CMsg结构，并获取其指针
     * ret: 返回CMsg结构指针
     */
    CSmartPtr<CMsg> getMsg();

    /*
     * 清空缓存
     */
    void clear();

    /*
     * 获取互斥锁
     */
    CMutex* getMutex();
    
private:
    UINT1* m_strData;
    INT4 m_iCur;
    INT4 m_iTail;
    INT4 m_iSize;
    CMutex m_oMutex;
};


#endif