#pragma once

#include "RCUSynchronizer.h"
#include "RBTree.h"

typedef struct {

    RCUSynchronizer *queueSynchronizer;

    unsigned long pendingEnqueueOperationsIndex;
    RBTree *pendingEnqueueOperations;
    spinlock_t pendingEnqueueOperationsSpinlock;

    unsigned long pendingDequeueOperationsIndex;
    RBTree *pendingDequeueOperations;
    spinlock_t pendingDequeueOperationsSpinlock;

    unsigned long enqueueDelay;
    unsigned long dequeueDelay;
} Session;

Session *allocateSession(RCUSynchronizer *queueSynchronizer);

void revokePendingDequeueOperations(Session *input);

void revokePendingEnqueueOperations(Session *input);

void freeSession(Session *input);

void registerDelayedEnqueueOperation(Session* input, DelayedEnqueueOperation* operation, void (* work)(struct work_struct*));

unregisterDelayedEnqueueOperation(Session* input, DelayedEnqueueOperation* operation);