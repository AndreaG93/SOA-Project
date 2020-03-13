#pragma once

#include "DataCell.h"

#define WAIT_FREE_QUEUE_NODE_SIZE 4

typedef struct {

    void *data;
    void *next;

} LockFreeQueueNode;

typedef struct {

    LockFreeQueueNode* tailNode;
    LockFreeQueueNode* headNode;


    long maxMessageSize;
    long maxStorageSize;
    long currentUsedStorage;

} LockFreeQueue;

LockFreeQueue *allocateAndInitializeLockFreeQueue(long maxMessageSize, long maxStorageSize);

unsigned int enqueue(LockFreeQueue *queue, void *data);

void *dequeue(LockFreeQueue *queue);