#include <linux/slab.h>

#include "ModuleFunctions.h"
#include "Common/BasicDefines.h"
#include "DataStructure/RBTree.h"
#include "DataStructure/RCUSynchronizer.h"
#include "DataStructure/SemiLockFreeQueue.h"
#include "DataStructure/Message.h"

RCUSynchronizer *getQueueRCUSynchronizer(RCUSynchronizer *RBTreeSynchronizer, unsigned long queueID) {

    RCUSynchronizer *output;
    unsigned long epoch;

    epoch = readLockRCUGettingEpoch(RBTreeSynchronizer);
    output = (RCUSynchronizer *) searchRBTree(RBTreeSynchronizer->RCUProtectedDataStructure, queueID);
    readUnlockRCU(RBTreeSynchronizer, epoch);

    return output;
}

RCUSynchronizer *allocateNewQueueRCUSynchronizer(void) {

    RCUSynchronizer *output;
    SemiLockFreeQueue *outputQueue;

    outputQueue = allocateSemiLockFreeQueue(DEFAULT_MAX_MESSAGE_SIZE, DEFAULT_MAX_STORAGE_SIZE, NULL);  // TODO KOBJECT
    if (outputQueue == NULL)
        return NULL;

    output = allocateRCUSynchronizer(outputQueue);
    if (output == NULL)
        freeSemiLockFreeQueue(outputQueue, &fullyRemoveMessage);

    return output;
}

void fullyRemoveMessage(void *input) {

    Message *message;

    message = (Message *) input;
    freeMessage(message);
}

void fullyRemoveQueue(void *input) {

    SemiLockFreeQueue *queue;

    queue = (SemiLockFreeQueue *) input;
    freeSemiLockFreeQueue(queue, &fullyRemoveMessage);
}

void fullyRemoveQueueSynchronizer(void *input) {

    RCUSynchronizer *queueSynchronizer;

    queueSynchronizer = (RCUSynchronizer *) input;
    freeRCUSynchronizer(queueSynchronizer, &fullyRemoveQueue);
}

void fullyRemoveRBTreeSynchronizer(RCUSynchronizer* input) {

    freeRBTreeContentIncluded(input->RCUProtectedDataStructure, &fullyRemoveQueueSynchronizer);
    kfree(input);
}