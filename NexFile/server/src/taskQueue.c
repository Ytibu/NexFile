#include "../include/taskQueue.h"

#include <stdlib.h>
#include <stdio.h>

#include "../../shared/logger.h"

int taskQueueInit(TaskQueue_t *taskQueue)
{
    taskQueue->queueSize_ = 0;
    taskQueue->phead_ = 0;
    taskQueue->ptail_ = 0;
    return 0;
}

int taskQueuePush(TaskQueue_t *pQueue, int netFd)
{
    node_t *newNode = (node_t *)malloc(sizeof(node_t));
    ERROR_CHECK(newNode, NULL, "malloc");

    newNode->netFd = netFd;
    if(pQueue->queueSize_ == 0) {
        pQueue->phead_ = newNode; // 将头索引指向新节点
        pQueue->ptail_ = newNode; // 将尾索引指向新节点
    } else {
        node_t *tailNode = (node_t *)(long)pQueue->ptail_; // 获取当前尾节点
        tailNode->Pnext_ = newNode; // 将当前尾节点的下一个节点指向新节点
        pQueue->ptail_ = newNode; // 更新尾索引指向新节点
    }

    ++pQueue->queueSize_; // 队列大小加1
    return 0;
}

int taskQueuePop(TaskQueue_t *pQueue)
{
    if(pQueue->queueSize_ == 0) {
        return -1; // 队列为空，无法弹出
    }

    node_t *pCur = pQueue->phead_; /*获取队列头节点*/
    pQueue->phead_ = pCur->Pnext_; /*更新头指针指向下一个节点*/
    if(pQueue->queueSize_ == 1){   /*如果队列只有一个节点，更新尾指针为NULL*/
        pQueue->ptail_ = NULL;
    }
    free(pCur);   /*释放原头节点的内存*/
    --pQueue->queueSize_;   /*更新队列大小*/
    return 0;

}