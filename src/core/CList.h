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
*   File Name   : CList.h           *
*   Author      : bnc zgzhao           *
*   Create Date : 2016-5-25           *
*   Version     : 1.0           *
*   Function    : .           *
*                                                                             *
******************************************************************************/
#ifndef __CLIST__H
#define __CLIST__H
#include "bnc-type.h"
#include "CRefObj.h"
#include "CMutex.h"

typedef unsigned int size_type;
/*
 * list封装类
 */
template<class T>
class CList: public CRefObj, public std::list<T>
{
public:
    CList():m_isBusy(FALSE),CRefObj(TRUE)
    {
    }

    ~CList()
    {
        clear();
    }

    void push_front(const T& x)
    {
        m_oMutex.lock();
        std::list<T>::push_front(x);
        m_oMutex.unlock();
    }

    void pop_front()
    {
        m_oMutex.lock();
        std::list<T>::pop_front();
        m_oMutex.unlock();
    }

    void push_back(const T& x)
    {
        m_oMutex.lock();
        std::list<T>::push_back(x);
        m_oMutex.unlock();
    }

    void pop_back()
    {
        m_oMutex.lock();
        std::list<T>::pop_back();
        m_oMutex.unlock();
    }

    size_t size()
    {
        size_t ret = 0;
        m_oMutex.lock();
        ret = std::list<T>::size();
        m_oMutex.unlock();
        return ret;
    }

    T* front()
    {
        T* ret = NULL;
        m_oMutex.lock();
        if (std::list<T>::size() > 0)
        {
            ret = &(std::list<T>::front());
        }
        m_oMutex.unlock();
        return ret;
    }

    T* back()
    {
        T* ret = NULL;
        m_oMutex.lock();
        if (std::list<T>::size() > 0)
        {
            ret = &(std::list<T>::back());
        }
        m_oMutex.unlock();
        return ret;
    }

    typename std::list<T>::iterator erase(typename std::list<T>::iterator position)
    {
        m_oMutex.lock();
        std::list<T>::erase(position);
        m_oMutex.unlock();
    }

    typename std::list<T>::iterator erase(typename std::list<T>::iterator first, typename std::list<T>::iterator last)
    {
        m_oMutex.lock();
        std::list<T>::erase(first, last);
        m_oMutex.unlock();
    }

    void clear()
    {
        m_oMutex.lock();
        std::list<T>::clear();
        m_oMutex.unlock();
    }

    /*
     * 是否正在被处理中
     * ret: 处理中 TRUE，空闲中 FALSE
     */
    BOOL isBusy()
    {
        return m_isBusy;
    }

    /*
     * 设置忙闲状态
     * paran: busy
     */
    void setBusy(BOOL busy)
    {
        m_isBusy = busy;
    }

private:
    BOOL m_isBusy;
    CMutex m_oMutex;
};


#endif
