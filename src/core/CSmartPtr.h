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
*   File Name   : CSmartPtr.h           *
*   Author      : bnc zgzhao           *
*   Create Date : 2016-5-25           *
*   Version     : 1.0           *
*   Function    : .           *
*                                                                             *
******************************************************************************/
#ifndef __CSMARTPTR_H
#define __CSMARTPTR_H
#include "bnc-type.h"

/*
 * 智能指针模板类
 */
template <typename T>
class CSmartPtr
{
public:
    explicit CSmartPtr(T* p = NULL)
    {
        m_pRawObj = p;
        if(p != NULL)
        {
            p->AddRefCount();
        }
    }

    CSmartPtr(const CSmartPtr& ref)
    {
        m_pRawObj = ref.m_pRawObj;
        if(m_pRawObj != NULL)
        {
            m_pRawObj->AddRefCount();
        }
    }

    ~CSmartPtr()
    {
        if(m_pRawObj != NULL && m_pRawObj->SubRefCount() == 0)
        {
            delete m_pRawObj;
        }
    }

    T* operator->() const
    {
        return m_pRawObj;
    }

    T& operator()() const
    {
        assert(m_pRawObj != NULL);
        return *m_pRawObj;
    }

    T& operator*() const
    {
        assert(m_pRawObj != NULL);
        return *m_pRawObj;
    }

    T* getPtr() const
    {
        return m_pRawObj;
    }

    bool isNull() const
    {
        return m_pRawObj == NULL;
    }

    CSmartPtr& operator = (const CSmartPtr& ref)
    {
        if(this != &ref)
        {
            if(m_pRawObj != NULL
                && m_pRawObj->SubRefCount() == 0)
            {
                delete m_pRawObj;
            }

            m_pRawObj = ref.m_pRawObj;

            if(m_pRawObj != NULL)
            {
                m_pRawObj->AddRefCount();
            }
        }

        return *this;
    }

    bool operator == (const CSmartPtr& ref) const
    {
        return m_pRawObj == ref.m_pRawObj;
    }

private:
    T* m_pRawObj;
};
#endif
