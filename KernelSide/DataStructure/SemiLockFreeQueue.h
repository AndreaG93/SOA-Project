#pragma once
#include <linux/kobject.h>

typedef struct {

    void *next;
    void *data;
    unsigned long dataSize;

} SemiLockFreeQueueNode;

typedef struct {

    SemiLockFreeQueueNode* tail;
    SemiLockFreeQueueNode* head;

    long maxMessageSize;
    long maxStorageSize;
    long currentUsedStorage;

    struct kobject *kObject;

} SemiLockFreeQueue;

SemiLockFreeQueue *allocateAndInitializeSemiLockFreeQueue(long maxMessageSize, long maxStorageSize, struct kobject *kObject);

unsigned int enqueue(SemiLockFreeQueue *queue, void *data);

void *dequeue(SemiLockFreeQueue *queue);

void freeSemiLockFreeQueue(SemiLockFreeQueue *queue);