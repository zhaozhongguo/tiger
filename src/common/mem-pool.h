/**
* @file  mem-pool.h
* @brief  内存池
* @author  zhaozhongguo
* @date  2016-9-13 
* @version  1.0
*/
#ifndef __MEM_POOL_H
#define __MEM_POOL_H
#include "type-def.h"

namespace common
{
    /** 
    * @brief 内存池
    */ 
    class CMemPool
    {
    public:
        CMemPool():m_pPool(NULL)
        {}

        ~CMemPool()
        {
            destroy();
        }

        bool create(uint32_t blockSize , uint32_t len);

        void* malloc();

        bool free(void *data);

        void destroy();

    private:
        void* m_pPool;
    };

}

#endif
