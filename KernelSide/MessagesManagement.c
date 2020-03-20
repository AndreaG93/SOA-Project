#include <linux/workqueue.h>
#include <linux/slab.h>


#include "MessagesManagement.h"
#include "DelayedOperations.h"

#include "Common/BasicDefines.h"

#include "DataStructure/RCUSynchronizer.h"
#include "DataStructure/Message.h"
#include "DataStructure/SemiLockFreeQueue.h"
#include "DataStructure/Session.h"

DriverError enqueueMessage(RCUSynchronizer* queueSynchronizer, const char *userBuffer, size_t userBufferSize) {

    DriverError output;
    Message *message;
    SemiLockFreeQueue *queue;
    unsigned int epoch;

    epoch = readLockRCUGettingEpoch(queueSynchronizer);

    queue = (SemiLockFreeQueue *) queueSynchronizer->RCUProtectedDataStructure;

    if (userBufferSize > queue->maxMessageSize)
        output = ERR_MESSAGE_TOO_BIG;
    else {

        message = allocateMessage(userBuffer, userBufferSize);
        if (message == NULL)
            output = ERR_ALLOCATION_FAILED;
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
        output = ERR_EMPTY_QUEUE;
    else {

        output = copyMessageToUserBuffer(message, userBuffer, userBufferSize);
        freeMessage(message);
    }

    readUnlockRCU(queueSynchronizer, epoch);
    return output;
}

void performDelayedMessageEnqueue(struct work_struct *input) {

    struct delayed_work *delayedWork;
    DelayedEnqueueOperation *operation;
    SemiLockFreeQueue *queue;
    unsigned int epoch;
    RCUSynchronizer* queueSynchronizer;

    delayedWork = container_of(input, struct delayed_work, work);
    operation = container_of(delayedWork, DelayedEnqueueOperation, work);

    unregisterDelayedEnqueueOperation(operation);

    queueSynchronizer = operation->session->queueSynchronizer;

    epoch = readLockRCUGettingEpoch(queueSynchronizer);

    queue = (SemiLockFreeQueue *) queueSynchronizer->RCUProtectedDataStructure;

    if (operation->message->size <= queue->maxMessageSize)
        if (SUCCESS != enqueue(queue, operation->message, operation->message->size))
            freeMessage(operation->message);

    readUnlockRCU(queueSynchronizer, epoch);

    kfree(operation);
}