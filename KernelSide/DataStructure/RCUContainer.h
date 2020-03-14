#pragma once

#include <linux/spinlock_types.h>

typedef struct {

    unsigned int epoch;
    unsigned long standing[2];
    spinlock_t writeLock;

    void *RCUProtectedData;

} RCUContainer;

RCUContainer *allocateRCUContainer(void *RCUProtectedData);