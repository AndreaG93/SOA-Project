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

typedef struct {

    unsigned long index;

    struct delayed_work work;
    Session *session;

    const char *userBuffer;
    unsigned long userBufferSize;

} DelayedEnqueueOperation;

Session *allocateSession(RCUSynchronizer *queueSynchronizer);

void revokePendingDequeueOperations(Session *input);

void revokePendingEnqueueOperations(Session *input);

void freeSession(Session *input);

void registerDelayedEnqueueOperation(Session* input, DelayedEnqueueOperation* operation, void (* work)(struct work_struct*));

void unregisterDelayedEnqueueOperation(Session* input, DelayedEnqueueOperation* operation);