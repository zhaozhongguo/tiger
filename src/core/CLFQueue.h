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
*   File Name   : CLFQueue.h           *
*   Author      : bnc xflu           *
*   Create Date : 2016-8-4           *
*   Version     : 1.0           *
*   Function    : .           *
*                                                                             *
******************************************************************************/
#ifndef _CLFQUEUE_H
#define _CLFQUEUE_H

#include "bnc-type.h"
#include "CRefObj.h"
#include <tbb/concurrent_queue.h>

/*
 * TBB提供的一种无锁队列
 * 来自Intel并行计算库
 */
typedef unsigned int size_type;

template<class T>
class CLFQueue: public CRefObj
{
public:
	CLFQueue(): m_isBusy(FALSE), CRefObj(TRUE)
    {
    }

    ~CLFQueue()
    {
    	clear();
    }

    void push(const T& x)
    {
    	q.push(x);
    }

    void push_back(const T& x)
    {
    	q.push(x);
    }

    void pop(T& x)
    {
    	q.pop(x);
    }

    BOOL try_push(const T& x)
	{
    	return q.try_push(x);
	}

	BOOL try_pop(T& x)
	{
		return q.try_pop(x);
	}

    BOOL empty()
    {
    	return q.empty();
    }

    /*
     * 是否正在被处理中
     * @return:  TRUE:处理中, FALSE:空闲
     */
    BOOL isBusy()
    {
        return m_isBusy;
    }

    /*
     * 设置忙闲状态
     * @param: busy
     */
    void setBusy(BOOL busy)
    {
        m_isBusy = busy;
    }

    void clear()
    {
    	q.clear();
    }

    /*
     * 非线程安全!
     * 谨慎使用
     */
    size_t size()
    {
    	return q.size();
    }

private:
    BOOL m_isBusy;
    tbb::concurrent_bounded_queue<T> q;
};

#endif




