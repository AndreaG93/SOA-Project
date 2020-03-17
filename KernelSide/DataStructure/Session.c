#include "Session.h"
#include "RCUSynchronizer.h"
#include "RBTree.h"

#include <linux/slab.h>
#include <linux/workqueue.h>

SessionData *allocateSession() {

    Session *output = kmalloc(sizeof(Session), GFP_KERNEL);
    if (output != NULL) {

        spin_lock_init(&output->spinlock);

        output->dequeueDelay = 0;
        output->enqueueDelay = 0;

        output->delayedMessages = allocateRBTree();
    }

    return output;
}

void revokeDelayedMessage(void *input) {

    struct delayed_work* delayedWork;

    cancel_delayed_work_sync(delayedWork);
    kfree(delayedWork);
}

void revokeDelayedMessages(Session *input) {

    spin_lock(&input->spinlock);

    freeRBTreeContentIncluded(input->delayedMessages, &revokeDelayedMessage)
    output->delayedMessages = allocateRBTree();

    spin_unlock(&input->spinlock);
}

void freeSession(Session* input) {

    freeRBTreeContentIncluded(input->pendingRequests, &revokeDelayedMessage);
    kfree(input);
}