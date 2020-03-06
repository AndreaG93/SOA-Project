#pragma once

#define WAIT_FREE_QUEUE_NODE_SIZE 4

typedef struct {
    volatile bool pending;
    volatile unsigned long targetCellIndex;
} RequestState;

typedef struct {
    volatile void *data;
    volatile RequestState state;
} EnqueueRequest;

typedef struct {
    volatile unsigned long identifier;
    volatile RequestState state;
} DequeueRequest;

typedef struct {
    volatile void *data;
    volatile EnqueueRequest *enqueueRequest;
    volatile DequeueRequest *dequeueRequest;
} Cell;

typedef struct _WaitFreeQueueNode {

    volatile long identifier;
    volatile struct _WaitFreeQueueNode *next;
    volatile Cell cells[];

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
        struct _ThreadLocalState* peer;
    } Enqueue;

    struct {
        DequeueRequest dequeueRequest;
        struct _ThreadLocalState* peer;
    } Dequeue;


} ThreadLocalState;