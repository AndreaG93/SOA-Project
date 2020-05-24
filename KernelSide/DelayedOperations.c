#include <linux/workqueue.h>
#include <linux/slab.h>
#include <linux/wait.h>
#include <linux/sched.h>

#include "DelayedOperations.h"
#include "DataStructure/Session.h"
#include "DataStructure/Message.h"

DelayedEnqueueOperation *
allocateDelayedEnqueueOperation(Session *session, const char *userBuffer, unsigned long userBufferSize) {

    DelayedEnqueueOperation *output;

    output = kmalloc(sizeof(DelayedEnqueueOperation), GFP_KERNEL);
    if (output != NULL) {
        output->index = (session->delayedEnqueueOperationsIndex)++;

        output->session = session;
        output->message = allocateMessage(userBuffer, userBufferSize);
        if (output->message == NULL) {
            kfree(output);
            return NULL;
        }
    }

    return output;
}

void registerAndEnableDelayedEnqueueOperation(DelayedEnqueueOperation *operation, void (*work)(struct work_struct *)) {

    Session *session = operation->session;

    spin_lock(&session->delayedEnqueueOperationsSpinlock);

    insertRBTree(session->delayedEnqueueOperations, operation->index, operation);

    INIT_DELAYED_WORK(&operation->work, work);
    schedule_delayed_work(&operation->work, session->enqueueDelay);

    spin_unlock(&session->delayedEnqueueOperationsSpinlock);
}

void unregisterDelayedEnqueueOperation(DelayedEnqueueOperation *operation) {

    Session *session = operation->session;

    spin_lock(&session->delayedEnqueueOperationsSpinlock);

    removeRBTree(session->delayedEnqueueOperations, operation->index, NULL);

    spin_unlock(&session->delayedEnqueueOperationsSpinlock);
}

void revokeDelayedEnqueueOperation(void *input) {

    struct delayed_work *delayedWork;
    DelayedEnqueueOperation *operation;

    delayedWork = container_of(input,
    struct delayed_work, work);
    operation = container_of(delayedWork, DelayedEnqueueOperation, work);

    if (cancel_delayed_work_sync(&operation->work)) {
        printk("Deleting 'DelayedEnqueueOperation'...");

        freeMessage(operation->message);
        kfree(operation);
    }
}

void revokeAllDelayedEnqueueOperations(Session *input) {

    spin_lock(&input->delayedEnqueueOperationsSpinlock);

    cleanRBTree(input->delayedEnqueueOperations, &revokeDelayedEnqueueOperation);

    spin_unlock(&input->delayedEnqueueOperationsSpinlock);
}

void revokeAllDelayedEnqueueOperationsVoid(void *input) {
    revokeAllDelayedEnqueueOperations((Session *) input);
}

void revokeAllDelayedDequeueOperationsVoid(void *input) {

    Session* session;

    session = (Session *) input;

    session->wakeUpFlag = WAKE_UP_FLAG;
    wake_up(session->waitQueueHead);
    session->wakeUpFlag = NOT_WAKE_UP_FLAG;
}