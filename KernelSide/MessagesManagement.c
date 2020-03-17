#include "MessagesManagement.h"

#include "Common/BasicDefines.h"

#include "DataStructure/RCUSynchronizer.h"
#include "DataStructure/Message.h"
#include "DataStructure/SemiLockFreeQueue.h"

#include <linux/workqueue.h>

unsigned int enqueueMessage(RCUSynchronizer* queueSynchronizer, const char *userBuffer, size_t userBufferSize) {

    Message *message;
    SemiLockFreeQueue *queue;

    unsigned long epoch;
    unsigned long output;

    epoch = readLockRCUGettingEpoch(queueSynchronizer);

    queue = (SemiLockFreeQueue *) queueSynchronizer->RCUProtectedDataStructure;

    if (userBufferSize > queue->maxMessageSize)
        output = FAILURE;
    else {

        message = createMessageFromUserBuffer(userBuffer, userBufferSize);
        output = enqueue(queue, message);

        if (output == FAILURE)
            freeMessage(message);
    }

    readUnlockRCU(queueSynchronizer, epoch);
    return output;
}

unsigned int dequeueMessage(RCUSynchronizer* queueSynchronizer, void *userBuffer, size_t userBufferSize) {

    Message *message;
    SemiLockFreeQueue *queue;

    unsigned long epoch;
    unsigned char output;

    epoch = readLockRCUGettingEpoch(queueSynchronizer);

    queue = (SemiLockFreeQueue *) queueSynchronizer->RCUProtectedDataStructure;

    message = dequeue(queue);

    output = copyMessageToUserBuffer(message, userBuffer, userBufferSize);

    freeMessage(message);
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