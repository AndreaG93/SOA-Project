#include <linux/slab.h>
#include <linux/workqueue.h>

#include "Session.h"
#include "RCUSynchronizer.h"
#include "RBTree.h"

Session *allocateSession(RCUSynchronizer *queueSynchronizer) {

    Session *output;
    RBTree *delayedEnqueueOperations;
    RBTree *delayedDequeueOperations;

    output = kmalloc(sizeof(Session), GFP_KERNEL);
    if (output == NULL)
        return NULL;

    delayedEnqueueOperations = allocateRBTree();
    if (delayedEnqueueOperations == NULL) {

        kfree(output);
        return NULL;
    }

    delayedDequeueOperations = allocateRBTree();
    if (delayedDequeueOperations == NULL) {

        kfree(delayedEnqueueOperations);
        kfree(output);
        return NULL;
    }

    spin_lock_init(&output->delayedDequeueOperationsSpinlock);
    spin_lock_init(&output->delayedEnqueueOperationsSpinlock);

    output->dequeueDelay = 0;
    output->enqueueDelay = 0;

    output->queueSynchronizer = queueSynchronizer;
    output->delayedDequeueOperations = delayedDequeueOperations;
    output->delayedEnqueueOperations = delayedEnqueueOperations;

    output->delayedDequeueOperationsIndex = 0;
    output->delayedEnqueueOperationsIndex = 0;

    return output;
}

void freeSession(Session *input) {

    spin_lock(&input->delayedEnqueueOperationsSpinlock);
    spin_lock(&input->delayedDequeueOperationsSpinlock);

    freeRBTreeContentIncluded(input->delayedEnqueueOperations, NULL);
    freeRBTreeContentIncluded(input->delayedDequeueOperations, NULL);

    spin_unlock(&input->delayedDequeueOperationsSpinlock);
    spin_unlock(&input->delayedEnqueueOperationsSpinlock);

    kfree(input);
}