#pragma once

#include <linux/spinlock_types.h>

typedef struct {

    unsigned int epoch;
    unsigned long standing[2];
    spinlock_t writeLock;

    void *RCUProtectedDataStructure;

} RCUSynchronizer;

RCUSynchronizer *allocateRCUSynchronizer(void *dataStructureToProtect);

unsigned int readLockRCUGettingEpoch(RCUSynchronizer *input);

void readUnlockRCU(RCUSynchronizer *input, unsigned int epoch);

void writeLockRCU(RCUSynchronizer *input);

void writeUnlockRCU(RCUSynchronizer *input, void *newDataStructureToProtect);