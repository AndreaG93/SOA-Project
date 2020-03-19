#pragma once

#include "RCUSynchronizer.h"
#include "RBTree.h"

typedef struct {

    RCUSynchronizer *queueSynchronizer;

    RBTree *pendingEnqueueOperations;
    RBTree *pendingDequeueOperations;

    spinlock_t pendingEnqueueOperationsSpinlock;
    spinlock_t pendingDequeueOperationsSpinlock;

    unsigned long enqueueDelay;
    unsigned long dequeueDelay;

} Session;

Session *allocateSession(RCUSynchronizer *queueSynchronizer);

void revokePendingDequeueOperations(Session *input);

void revokePendingEnqueueOperations(Session *input);

void freeSession(Session *input);