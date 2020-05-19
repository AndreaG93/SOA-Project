#pragma once

#include <linux/wait.h>

#include "RCUSynchronizer.h"
#include "RBTree.h"

typedef struct {

    RCUSynchronizer *queueSynchronizer;

    unsigned long enqueueDelay;
    unsigned long dequeueDelay;

    RBTree *delayedEnqueueOperations;
    spinlock_t delayedEnqueueOperationsSpinlock;

    struct wait_queue_head *waitQueueHead;
    struct wait_queue_entry *delayedDequeueOperation;
    unsigned int wakeUpFlag;

    unsigned long delayedEnqueueOperationsIndex;

} __randomize_layout
Session;


Session *allocateSession(RCUSynchronizer *queueSynchronizer);

void freeSession(Session *input);