/**
* @file  mem-pool.cpp
* @brief  内存池
* @author  zhaozhongguo
* @date  2016-9-13 
* @version  1.0
*/
#include <pthread.h>
#include <stdlib.h>
#include "mem-pool.h"
#include "log.h"

namespace common
{
    //内存池的操作
    typedef struct QueueList          //内存池队列
    {
        uint32_t       totalLen;      //总共的单元个数
        uint32_t       usableLen;     //可用的
        void*          data;          //内存池首地址
        void**         arr;           //存储每块内存的首地址
        uint32_t       head;          //队列头指针
        uint32_t       tail;          //队列尾指针
        void*          mutex;         //锁
        char*          states;        //防止重复释放,0:没有占用  1:已经占用
        uint16_t       blockSize;     //内存块大小
    }QueueList_t;


    //入队列
    static bool enqueue(void* list, void* data)
    {
        if(NULL == data || NULL == list)
        {
            return false;
        }

        QueueList_t* qList = (QueueList_t*)list;
        //必须为0
        if( (data - qList->data) %  qList->blockSize )
        {
            LOG_ERROR_FMT("mem pool enqueue %p failed, invalid address!", data);
            return false;
        }

        //节点的位置
        int pos = (data - qList->data) /  qList->blockSize;      
        if(pos < 0 || pos >= qList->totalLen )
        {
            LOG_ERROR_FMT("mem pool enqueue %p failed, overflow!", data);
            return false;
        }

        //说明重复释放了内存
        if(1 == *(qList->states + pos))                 
        {
            LOG_ERROR_FMT("mem pool enqueue %p failed, already free!", data);
            return false;
        }

        pthread_mutex_lock(qList->mutex);
        {
            uint32_t tail = qList->tail;
            qList->arr[tail] = data;
            *(qList->states + pos) = 1;
            qList->tail = (tail + 1) % qList->totalLen;
            qList->usableLen++;
        }
        pthread_mutex_unlock(qList->mutex);

        return true;
    }


    //出队列
    static void* dequeue(void* list)
    {
        if (NULL == list)
        {
            LOG_ERROR_FMT("mem pool dequeue %p failed, invalid param!", data);
            return NULL;
        }

        QueueList_t* qList = (QueueList_t*)(list);
        void* unit = NULL;
        pthread_mutex_lock(qList->mutex);
        {
            if (qList->usableLen > 0)
            {
                uint32_t head = qList->head;
                unit = qList->arr[head];
                int pos = (unit - qList->data)  /  qList->blockSize;
                *(qList->states + pos) = 0;
                qList->usableLen--;
                qList->head = (head+1)%qList->totalLen;
                memset(unit, 0, qList->blockSize);
            }
        }
        pthread_mutex_unlock(qList->mutex);
        return unit;
    }


    static void* initQueue(uint32_t blockSize, uint32_t len)
    {
        QueueList_t *qList = (QueueList_t *)malloc(sizeof(QueueList_t));
        if(NULL == qList) 
        {
            LOG_ERROR_FMT("mem pool initQueue %p failed, malloc queue failed!", data);
            return NULL;
        }
        memset(qList, 0, sizeof(QueueList_t));

        //锁
        qList->mutex = (pthread_mutex_t*)malloc(sizeof(pthread_mutex_t));
        if(NULL == qList->mutex)
        {
            LOG_ERROR_FMT("mem pool initQueue %p failed, malloc mutex failed!", data);
            return NULL;
        }   
        pthread_mutex_init(qList->mutex, NULL);
        //首地址
        qList->data = malloc(blockSize * len);
        if(NULL == qList->data)
        {
            LOG_ERROR_FMT("mem pool initQueue %p failed, malloc pool failed!", data);
            return NULL;
        }
        memset(qList->data, 0, blockSize * len);

        qList->arr = (void**)malloc(len * sizeof(void *));
        if(NULL == qList->arr)
        {
            LOG_ERROR_FMT("mem pool initQueue %p failed, malloc arr failed!", data);
            return NULL;
        }
        memset(qList->arr, 0, len * sizeof(void*));

        qList->states = (char*)malloc(len * sizeof(char));
        if(NULL == qList->states)
        {
            LOG_ERROR_FMT("mem pool initQueue %p failed, malloc states failed!", data);
            return NULL;
        }
        memset(qList->states, 0, len * sizeof(char));

        qList->head = 0;
        qList->tail = 0;
        qList->usableLen = 0;
        qList->blockSize = blockSize;
        qList->totalLen = len;

        char* data = NULL;
        for(int i=0; i < len; i++)
        {
            data = qList->data + i * blockSize;
            enqueue(qList, data);
        }

        return (void*)qList;
    }


    bool CMemPool::create(uint32_t blockSize, uint32_t len)
    {
        m_pPool = initQueue(blockSize, len);
        if (NULL == m_pPool)
        {
            return false;
        }

        return true;
    }


    void* CMemPool::malloc()
    {
        return dequeue(m_pPool);
    }


    bool CMemPool::free(void* data)
    {
        return enqueue(m_pPool, data);
    }


    void CMemPool::destroy()
    {
        QueueList_t* qList = (QueueList_t*)m_pPool;
        if (qList)
        {
            if (qList->arr)
            {
                ::free(qList->arr);
            }

            if (qList->data)
            {
                ::free(qList->data);
            }

            if (qList->states)
            {
                ::free(qList->states);
            }

            if(qList->mutex)
            {
                pthread_mutex_destroy(qList->mutex);
                ::free(qList->mutex);
            }

            ::free(qList);
        }
    }
}