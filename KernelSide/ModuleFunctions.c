#include <linux/slab.h>
#include <linux/kobject.h>
#include <linux/sysfs.h>

#include "ModuleFunctions.h"

#include "Common/BasicOperations.h"
#include "Common/BasicDefines.h"

#include "DataStructure/RBTree.h"
#include "DataStructure/RCUSynchronizer.h"
#include "DataStructure/SemiLockFreeQueue.h"
#include "DataStructure/Message.h"

void fullyRemoveMessage(void *input) {

    Message *message;

    message = (Message *) input;
    freeMessage(message);
}

void fullyRemoveQueue(void *input) {

    SemiLockFreeQueue *queue;

    queue = (SemiLockFreeQueue *) input;

    //kobject_put(queue->kObject);
    freeSemiLockFreeQueue(queue, &fullyRemoveMessage);
}

void fullyRemoveQueueSynchronizer(void *input) {

    RCUSynchronizer *queueSynchronizer;

    queueSynchronizer = (RCUSynchronizer *) input;
    freeRCUSynchronizer(queueSynchronizer, &fullyRemoveQueue);
}

void fullyRemoveRBTreeSynchronizer(RCUSynchronizer *input) {

    //freeRBTreeContentIncluded(input->RCUProtectedDataStructure, &fullyRemoveQueueSynchronizer);
    //kfree(input);
}


