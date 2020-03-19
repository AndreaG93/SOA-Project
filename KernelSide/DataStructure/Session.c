#include <linux/slab.h>
#include <linux/workqueue.h>

#include "Session.h"
#include "RCUSynchronizer.h"
#include "RBTree.h"

Session *allocateSession(RCUSynchronizer *queueSynchronizer) {

    Session *output;
    RBTree *pendingEnqueueOperations;
    RBTree *pendingDequeueOperations;

    output = kmalloc(sizeof(Session), GFP_KERNEL);
    if (output == NULL)
        return NULL;

    pendingEnqueueOperations = allocateRBTree();
    if (pendingEnqueueOperations == NULL) {

        kfree(output);
        return NULL;
    }

    pendingDequeueOperations = allocateRBTree();
    if (pendingDequeueOperations == NULL) {

        kfree(pendingEnqueueOperations);
        kfree(output);
        return NULL;
    }

    spin_lock_init(&output->pendingDequeueOperationsSpinlock);
    spin_lock_init(&output->pendingEnqueueOperationsSpinlock);

    output->dequeueDelay = 0;
    output->enqueueDelay = 0;

    output->queueSynchronizer = queueSynchronizer;
    output->pendingDequeueOperations = pendingDequeueOperations;
    output->pendingEnqueueOperations = pendingEnqueueOperations;

    output->pendingDequeueOperationsIndex = 0;
    output->pendingEnqueueOperationsIndex = 0;

    return output;
}

void revokePendingEnqueue(void *input) {

    struct delayed_work *pendingEnqueue;

    pendingEnqueue = (struct delayed_work *) input;

    cancel_delayed_work_sync(pendingEnqueue);
    kfree(pendingEnqueue);
}

void revokePendingDequeue(void *input) {
    // TODO
}

void revokePendingEnqueueOperations(Session *input) {

    spin_lock(&input->pendingEnqueueOperationsSpinlock);

    cleanRBTree(input->pendingEnqueueOperations, &revokePendingEnqueue);

    spin_unlock(&input->pendingEnqueueOperationsSpinlock);
}

void revokePendingDequeueOperations(Session *input) {
    // TODO
}

void freeSession(Session *input) {

    spin_lock(&input->pendingEnqueueOperationsSpinlock);
    freeRBTreeContentIncluded(input->pendingEnqueueOperations, &revokePendingEnqueue);
    spin_unlock(&input->pendingEnqueueOperationsSpinlock);

    spin_lock(&input->pendingDequeueOperationsSpinlock);
    freeRBTreeContentIncluded(input->pendingDequeueOperations, &revokePendingDequeue);
    spin_unlock(&input->pendingDequeueOperationsSpinlock);

    kfree(input);
}

void registerDelayedEnqueueOperation(Session* input, DelayedEnqueueOperation* operation, void (* work)(struct work_struct*)) {

    spin_lock(&input->pendingEnqueueOperationsSpinlock);

    operation->index = input->pendingEnqueueOperationsIndex;
    input->pendingEnqueueOperationsIndex++;

    insertRBTree(input->pendingEnqueueOperations, operation->index, operation);

    INIT_DELAYED_WORK(&operation->work, work);
    schedule_delayed_work(&operation->work, input->enqueueDelay);

    spin_unlock(&input->pendingEnqueueOperationsSpinlock);
}

void unregisterDelayedEnqueueOperation(Session* input, DelayedEnqueueOperation* operation) {

    spin_lock(&input->pendingEnqueueOperationsSpinlock);

    removeRBTree(input->pendingEnqueueOperations, operation->index, NULL);

    spin_unlock(&input->pendingEnqueueOperationsSpinlock);
}