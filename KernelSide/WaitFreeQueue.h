#pragma once
#include <linux/kobject.h>

typedef struct {

    unsigned int maxMessageSize;
    unsigned int maxStorageSize;

    struct kobject *kObject;

} WaitFreeQueue;

WaitFreeQueue* allocateAndInitializeWaitFreeQueue(unsigned long maxMessageSize, unsigned long maxStorageSize, struct kobject *kObject);




