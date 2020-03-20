#include <linux/slab.h>
#include <linux/workqueue.h>
#include <linux/wait.h>
#include <linux/sched.h>

#include "Session.h"
#include "RCUSynchronizer.h"
#include "RBTree.h"

Session *allocateSession(RCUSynchronizer *queueSynchronizer) {

    Session *output;
    RBTree *outputDelayedEnqueueOperations;
    struct wait_queue_head *outputWaitQueueHead;
    struct wait_queue_entry *outputDelayedDequeueOperation;

    output = kmalloc(sizeof(Session), GFP_KERNEL);
    if (output == NULL)
        return NULL;

    outputWaitQueueHead = kmalloc(sizeof(struct wait_queue_head), GFP_KERNEL);
    if (outputWaitQueueHead == NULL) {

        kfree(output);
        return NULL;
    }

    outputDelayedDequeueOperation = kmalloc(sizeof(struct wait_queue_entry), GFP_KERNEL);
    if (outputDelayedDequeueOperation == NULL) {

        kfree(outputWaitQueueHead);
        kfree(output);
        return NULL;
    }

    outputDelayedEnqueueOperations = allocateRBTree();
    if (outputDelayedEnqueueOperations == NULL) {

        kfree(outputDelayedDequeueOperation);
        kfree(outputWaitQueueHead);
        kfree(output);
        return NULL;
    }

    init_waitqueue_head(outputWaitQueueHead);
    init_waitqueue_entry(outputDelayedDequeueOperation, current);

    spin_lock_init(&output->delayedEnqueueOperationsSpinlock);

    output->dequeueDelay = 0;
    output->enqueueDelay = 0;

    output->queueSynchronizer = queueSynchronizer;
    output->delayedDequeueOperation = outputDelayedDequeueOperation;
    output->delayedEnqueueOperations = outputDelayedEnqueueOperations;
    output->waitQueueHead = outputWaitQueueHead;

    output->delayedEnqueueOperationsIndex = 0;

    return output;
}

void freeSession(Session *input) {

    freeRBTreeContentIncluded(input->delayedEnqueueOperations, NULL);

    kfree(input->delayedDequeueOperation);
    kfree(input->waitQueueHead);

    kfree(input);
}