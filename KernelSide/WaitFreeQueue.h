#pragma once

typedef struct {

    unsigned int maxMessageSize;
    unsigned int maxStorageSize;

    struct kobject *kObject;
    struct kobj_attribute *kObjectAttributes;

} WaitFreeQueue;

WaitFreeQueue* allocateAndInitializeWaitFreeQueue(void);




