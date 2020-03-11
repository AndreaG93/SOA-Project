#pragma once


#include "DataCell.h"

#define WAIT_FREE_QUEUE_NODE_SIZE 4

typedef struct {

    unsigned long identifier;
    void *next;
    DataCell cells[];

} WaitFreeQueueNode;

typedef struct {

    WaitFreeQueueNode *nodeList;

    unsigned long headIndex;
    unsigned long tailIndex;

    unsigned long maxMessageSize;
    unsigned long maxStorageSize;

} WaitFreeQueue;

typedef struct _ThreadLocalState {

    WaitFreeQueueNode *tailNode;
    WaitFreeQueueNode *headNode;

    struct _ThreadLocalState *next;

    struct {
        EnqueueRequest enqueueRequest;
        struct _ThreadLocalState *peer;
    } Enqueue;

    struct {
        DequeueRequest dequeueRequest;
        struct _ThreadLocalState *peer;
    } Dequeue;


} ThreadLocalState;

ThreadLocalState *allocateAndInitializeThreadLocalState(WaitFreeQueue *waitFreeQueue);

WaitFreeQueue *allocateAndInitializeWaitFreeQueue();

void enqueue(WaitFreeQueue *waitFreeQueue, ThreadLocalState *threadLocalState, void *data);