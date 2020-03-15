
#include "ModuleFunctions.h"
#include "./BasicOperations/BasicDefines.h"
#include "./DataStructure/RBTree.h"
#include "./DataStructure/RCUSynchronizer.h"
#include "./DataStructure/SemiLockFreeQueue.h"

RCUSynchronizer *getQueueRCUSynchronizer(RCUSynchronizer *RBTreeSynchronizer, unsigned long queueID) {

    RCUSynchronizer *output;
    unsigned long epoch;

    epoch = readLockRCUGettingEpoch(RBTreeSynchronizer);
    output = (RCUSynchronizer *) searchRBTree(RBTreeSynchronizer->RCUProtectedDataStructure, queueSynchronizerID);
    readUnlockRCU(RBTreeSynchronizer, epoch);

    return output;
}

RCUSynchronizer *allocateNewQueueRCUSynchronizer(void) {

    SemiLockFreeQueue *outputQueue = allocateSemiLockFreeQueue(DEFAULT_MAX_MESSAGE_SIZE, DEFAULT_MAX_STORAGE_SIZE, NULL);  // TODO KOBJECT
    if (outputQueue == NULL)
        return NULL;

    RCUSynchronizer *output = allocateRCUSynchronizer(outputQueue);
    if (output == NULL)
        freeSemiLockFreeQueue(outputQueue);

    return output;
}