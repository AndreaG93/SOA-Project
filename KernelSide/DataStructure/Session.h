#pragma once

#include "RCUSynchronizer.h"
#include "RBTree.h"

typedef struct {

    RCUSynchronizer *queueSynchronizer;

    unsigned long delayedEnqueueOperationsIndex;
    unsigned long delayedDequeueOperationsIndex;

    RBTree *delayedEnqueueOperations;
    RBTree *delayedDequeueOperations;

    spinlock_t delayedEnqueueOperationsSpinlock;
    spinlock_t delayedDequeueOperationsSpinlock;

    unsigned long enqueueDelay;
    unsigned long dequeueDelay;

} Session;



Session *allocateSession(RCUSynchronizer *queueSynchronizer);

void freeSession(Session *input);