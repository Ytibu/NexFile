#ifndef __TASK_QUEUE_H__
#define __TASK_QUEUE_H__


typedef struct node_s{
    int netFd; // 任务数据
    struct node_s *Pnext_; // 指向下一个节点的指针
} node_t;

typedef struct TaskQueue_s {
    int queueSize_; // 队列大小
    node_t *phead_; // 头索引
    node_t *ptail_; // 尾索引
} TaskQueue_t;

// 初始化任务队列
int taskQueueInit(TaskQueue_t *pQueue);

int taskQueuePush(TaskQueue_t *pQueue, int netFd);
int taskQueuePop(TaskQueue_t *pQueue);
#endif