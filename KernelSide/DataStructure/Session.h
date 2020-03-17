#pragma once

#include "RCUSynchronizer.h"
#include "RBTree.h"

typedef struct {

    spinlock_t spinlock;
    RBTree *delayedMessages;
    RCUSynchronizer *queueSynchronizer;

    unsigned long enqueueDelay;
    unsigned long dequeueDelay;

} Session;

Session* allocateSession(RCUSynchronizer *queueSynchronizer);

void revokeDelayedMessages(Session *input);

void freeSession(Session* input);