#pragma once

#include "../Hardware/MemoryAlignment.h"
#include "DataCell.h"


#define WAIT_FREE_QUEUE_NODE_SIZE 4

typedef struct _WaitFreeQueueNode {

    volatile unsigned long identifier;
    volatile struct _WaitFreeQueueNode *next;
    DataCell cells[];

} WaitFreeQueueNode;

typedef struct {

    volatile WaitFreeQueueNode *nodeList;
    volatile long headIndex;
    volatile long tailIndex;

} WaitFreeQueue;

typedef struct _ThreadLocalState {

    volatile WaitFreeQueueNode *tailNode, *headNode;
    volatile struct _ThreadLocalState *next;

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