#include <linux/workqueue.h>
#include <linux/slab.h>

#include "MessagesManagement.h"

#include "Common/BasicDefines.h"

#include "DataStructure/RCUSynchronizer.h"
#include "DataStructure/Message.h"
#include "DataStructure/SemiLockFreeQueue.h"
#include "DataStructure/DelayedEnqueueOperation.h"
#include "DataStructure/Session.h"

DriverError enqueueMessage(RCUSynchronizer* queueSynchronizer, const char *userBuffer, size_t userBufferSize) {

    DriverError output;
    Message *message;
    SemiLockFreeQueue *queue;
    unsigned int epoch;

    epoch = readLockRCUGettingEpoch(queueSynchronizer);

    queue = (SemiLockFreeQueue *) queueSynchronizer->RCUProtectedDataStructure;

    if (userBufferSize > queue->maxMessageSize)
        output = -FAILURE;
    else {

        message = allocateMessage(userBuffer, userBufferSize);
        if (message == NULL)
            output = -FAILURE;
        else {

            output = enqueue(queue, message, message->size);

            if (output == FAILURE)
                freeMessage(message);
        }
    }

    readUnlockRCU(queueSynchronizer, epoch);
    return output;
}

DriverError dequeueMessage(RCUSynchronizer* queueSynchronizer, void *userBuffer, size_t userBufferSize) {

    DriverError output;
    Message *message;
    SemiLockFreeQueue *queue;
    unsigned int epoch;

    epoch = readLockRCUGettingEpoch(queueSynchronizer);

    queue = (SemiLockFreeQueue *) queueSynchronizer->RCUProtectedDataStructure;

    message = dequeue(queue, &getMessageSize);
    if (message == NULL)
        output = -FAILURE;
    else {

        output = copyMessageToUserBuffer(message, userBuffer, userBufferSize);
        freeMessage(message);
    }

    readUnlockRCU(queueSynchronizer, epoch);
    return output;
}

void performDelayedEnqueueOperation(struct work_struct *input) {

    struct delayed_work *delayedWork;
    DelayedEnqueueOperation *operation;

    delayedWork = container_of(input, struct delayed_work, work);
    operation = container_of(delayedWork, DelayedEnqueueOperation, work);

    enqueueMessage(operation->session->queueSynchronizer, operation->userBuffer, operation->userBufferSize);
}

DriverError enqueueDelayedMessage(Session* session, const char *userBuffer, size_t userBufferSize) {

    DelayedEnqueueOperation *operation;

    operation = kmalloc(sizeof(DelayedEnqueueOperation), GFP_KERNEL);
    if (operation == NULL)
        return -FAILURE;

    operation->session = session;
    operation->userBuffer = userBuffer;
    operation->userBufferSize = userBufferSize;

    INIT_DELAYED_WORK(&operation->work, &performDelayedEnqueueOperation);
    schedule_delayed_work(&operation->work, session->enqueueDelay);

    return SUCCESS;
}