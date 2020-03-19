#include <linux/kobject.h>
#include <linux/spinlock_types.h>
#include <linux/sched.h>

#include "../Common/BasicDefines.h"

#include "SemiLockFreeQueue.h"
#include "KObjectManagement.h"
#include "RCUSynchronizer.h"
#include "RBTree.h"
#include "DeviceFileInstance.h"
#include "Session.h"

DeviceFileInstance *allocateDeviceFileInstance(unsigned int minorDeviceNumber,
                                               ssize_t (*show)(struct kobject *, struct kobj_attribute *, char *),
                                               ssize_t (*store)(struct kobject *, struct kobj_attribute *, const char *,
                                                                size_t)) {

    DeviceFileInstance *output;
    SemiLockFreeQueue *outputSemiLockFreeQueue;
    RCUSynchronizer *outputRCUSynchronizer;
    RBTree *outputRBTree;
    struct kobject *outputKObject;

    output = kmalloc(sizeof(DeviceFileInstance), GFP_KERNEL);
    if (output == NULL)
        return NULL;

    outputRBTree = allocateRBTree();
    if (outputRBTree == NULL) {

        kfree(output);

        return NULL;
    }

    outputSemiLockFreeQueue = allocateSemiLockFreeQueueKObject(DEFAULT_MAX_MESSAGE_SIZE, DEFAULT_MAX_STORAGE_SIZE);
    if (outputSemiLockFreeQueue == NULL) {

        freeRBTreeContentIncluded(outputRBTree, NULL);
        kfree(output);

        return NULL;
    }

    outputKObject = allocateKObject(convertIntToString(minorDeviceNumber), show, store);
    if (outputKObject == NULL) {

        freeSemiLockFreeQueue(outputSemiLockFreeQueue, NULL);
        freeRBTreeContentIncluded(outputRBTree, NULL);
        kfree(output);

        return NULL;
    }

    outputRCUSynchronizer = allocateRCUSynchronizer(outputQueue);
    if (output == NULL) {

        freeKObject(outputKObject);
        freeSemiLockFreeQueue(outputSemiLockFreeQueue, NULL);
        freeRBTreeContentIncluded(outputRBTree, NULL);
        kfree(output);

        return NULL;
    }

    spin_lock_init(&output->activeSessionsSpinlock)
    output->activeSessions = outputRBTree;
    output->semiLockFreeQueueRCUSynchronizer = outputRCUSynchronizer;
    output->KObject = outputKObject;

    return output;
}

DeviceFileInstance *getDeviceFileInstanceFromSynchronizer(RCUSynchronizer *input, unsigned int minorDeviceNumber) {

    DeviceFileInstance *output;
    unsigned long epoch;

    epoch = readLockRCUGettingEpoch(input);
    output = (DeviceFileInstance *) searchRBTree(input->RCUProtectedDataStructure, minorDeviceNumber);
    readUnlockRCU(input, epoch);

    return output;
}

void registerSessionIntoDeviceFileInstance(DeviceFileInstance *input, Session *session) {

    spin_lock(&input->activeSessionsSpinlock);

    insertRBTree(input->activeSessions, current->pid, session);

    spin_unlock(&input->activeSessionsSpinlock);
}