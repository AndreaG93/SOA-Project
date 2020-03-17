#pragma once

#include "RCUSynchronizer.h"
#include "RBTree.h"

typedef struct {

    spinlock_t spinlock;
    RBTree *delayedMessages;

    unsigned long enqueueDelay;
    unsigned long dequeueDelay;

} Session;

Session* allocateSession();

void revokeDelayedMessages(Session *input);

void freeSession(Session* input);