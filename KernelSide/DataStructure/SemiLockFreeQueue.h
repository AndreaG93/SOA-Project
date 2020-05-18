#pragma once

#include <linux/kobject.h>

#include "../Common/BasicDefines.h"

typedef struct {

    void *tail __attribute__((aligned(64)));
    void *head  __attribute__((aligned(64)));
    long currentUsedStorage __attribute__((aligned(64)));

    long maxMessageSize;
    long maxStorageSize;

} SemiLockFreeQueue __attribute__((aligned(64)));

SemiLockFreeQueue *allocateSemiLockFreeQueue(long maxMessageSize, long maxStorageSize);

DriverError enqueue(SemiLockFreeQueue *queue, void *data, unsigned long dataSize);

void *dequeue(SemiLockFreeQueue *queue, unsigned long (*getSizeFromData)(void *));

void freeSemiLockFreeQueue(SemiLockFreeQueue *queue, void (*freeFunction)(void *));