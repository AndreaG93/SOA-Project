#pragma once

#include <linux/spinlock_types.h>

typedef struct {

    unsigned int epoch;
    unsigned long standing[2];
    spinlock_t writeLock;

    void *data;

} DataRCUProtector;

RCUContainer *allocateRCUContainer(void *RCUProtectedData);

unsigned int readLockRCUGettingCurrentEpoch(RCUContainer *input);

void readUnlockRCU(RCUContainer *input, unsigned int threadEpoch);

void writeLockRCU(RCUContainer *input);

void writeUnlockRCU(RCUContainer *input, void *newRCUProtectedData);