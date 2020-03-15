#pragma once
#include <linux/kobject.h>

typedef struct {

    void* tail;
    void* head;

    long maxMessageSize;
    long maxStorageSize;
    long currentUsedStorage;

    struct kobject *kObject;

} SemiLockFreeQueue;

SemiLockFreeQueue *allocateSemiLockFreeQueue(long maxMessageSize, long maxStorageSize, struct kobject *kObject);

unsigned char enqueue(SemiLockFreeQueue *input, void *data);

void *dequeue(SemiLockFreeQueue *input);

void freeSemiLockFreeQueue(SemiLockFreeQueue *queue, void (*dataFreeFunction)(void *))