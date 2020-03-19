#pragma once

#include <linux/kobject.h>

#include "../Common/BasicDefines.h"

typedef struct {

    void *tail;
    void *head;

    long maxMessageSize;
    long maxStorageSize;
    long currentUsedStorage;

} SemiLockFreeQueue;

SemiLockFreeQueue *allocateSemiLockFreeQueue(long maxMessageSize, long maxStorageSize);

DriverError enqueue(SemiLockFreeQueue *queue, void *data, unsigned long dataSize);

void *dequeue(SemiLockFreeQueue *queue, unsigned long (*getSizeFromData)(void *));

void freeSemiLockFreeQueue(SemiLockFreeQueue *queue, void (*freeFunction)(void *));