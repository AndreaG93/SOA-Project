#include <linux/workqueue.h>

#include "MessagesManagement.h"

#include "Common/BasicDefines.h"

#include "DataStructure/RCUSynchronizer.h"
#include "DataStructure/Message.h"
#include "DataStructure/SemiLockFreeQueue.h"

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

void enqueueMessageDelayed(struct work_struct *input) {

    struct delayed_work *delayedWork;
    DelayedEnqueueMessageOperation *operation;

    delayedWork = container_of(input, struct delayed_work, work);
    operation = container_of(delayedWork, DelayedEnqueueMessageOperation, work);

    enqueueMessage(operation->queueSynchronizer, operation->userBuffer, operation->userBufferSize);
}