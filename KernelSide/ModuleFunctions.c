#include <linux/slab.h>
#include <linux/kobject.h>
#include <linux/sysfs.h>

#include "ModuleFunctions.h"
#include "Common/BasicOperations.h"
#include "Common/BasicDefines.h"
#include "Common/KObjectManagement.h"
#include "DataStructure/RBTree.h"
#include "DataStructure/RCUSynchronizer.h"
#include "DataStructure/SemiLockFreeQueue.h"
#include "DataStructure/Message.h"


struct sysfs_ops *
allocateAndInitializeSysFileSystemOperation(ssize_t (*show)(struct kobject *, struct attribute *, char *),
                                            ssize_t (*store)(struct kobject *, struct attribute *, const char *,
                                                             size_t)) {

    struct sysfs_ops *output = kmalloc(sizeof(struct sysfs_ops), GFP_KERNEL);
    if (output != NULL) {

        output->show = show;
        output->store = store;
    }

    return output;
}

struct kobject *
allocateSemiLockFreeQueueKObject(unsigned long queueID, struct kobject * kObjectParent, ssize_t (*show)(struct kobject *, struct attribute *, char *),
                                 ssize_t (*store)(struct kobject *, struct attribute *, const char *, size_t)) {

    char *kObjectName;
    struct kobject *kObject;
    struct kobj_type *kType;
    const struct attribute_group **attributesGroups;
    struct sysfs_ops *sysFSOperations;

    kObjectName = convertIntToString(queueID);

    kObject = kobject_create_and_add(kObjectName, kObjectParent);
    if (kObject == NULL)
        return NULL;

    kType = kmalloc(sizeof(struct kobj_type), GFP_KERNEL);
    if (kType == NULL) {

        kobject_put(kObject);
        kfree(kObject);
    }

    attributesGroups = kmalloc(sizeof(const struct attribute_group *), GFP_KERNEL);
    if (attributesGroups == NULL) {

        kobject_put(kObject);
        kfree(kObject);
    }

    attributesGroups[0] = allocateAttributeGroup(S_IWUSR | S_IRWXG, 2, "max_message_size", "max_storage_size");
    if (attributesGroups == NULL) {

        kobject_put(kObject);
        kfree(kObject);
        kfree(kType);
    }

    sysFSOperations = allocateAndInitializeSysFileSystemOperation(show, store);
    if (sysFSOperations == NULL) {

        freeAttributeGroup(attributesGroups[0], 2);
        kobject_put(kObject);
        kfree(kObject);
        kfree(kType);
    }

    kType->default_groups = attributesGroups;
    kType->sysfs_ops = sysFSOperations;
    kObject->ktype = kType;

    createAttributeGroupSysFiles(kObject, 2);

    return kObject;
}

RCUSynchronizer *getQueueRCUSynchronizer(RCUSynchronizer *RBTreeSynchronizer, unsigned long queueID) {

    RCUSynchronizer *output;
    unsigned long epoch;

    epoch = readLockRCUGettingEpoch(RBTreeSynchronizer);
    output = (RCUSynchronizer *) searchRBTree(RBTreeSynchronizer->RCUProtectedDataStructure, queueID);
    readUnlockRCU(RBTreeSynchronizer, epoch);

    return output;
}

RCUSynchronizer *
allocateNewQueueRCUSynchronizer(unsigned long queueID, struct kobject * kObjectParent,
                                ssize_t (*show)(struct kobject *, struct attribute *, char *),
                                ssize_t (*store)(struct kobject *, struct attribute *, const char *, size_t)) {

    RCUSynchronizer *output;
    SemiLockFreeQueue *outputQueue;
    struct kobject *kObject;

    kObject = allocateSemiLockFreeQueueKObject(queueID, kObjectParent, show, store);
    if (kObject == NULL)
        return NULL;

    outputQueue = allocateSemiLockFreeQueue(DEFAULT_MAX_MESSAGE_SIZE, DEFAULT_MAX_STORAGE_SIZE, kObject);
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

    freeKObjectRemovingFromSystem(queue->kObject, 2);
    freeSemiLockFreeQueue(queue, &fullyRemoveMessage);
}

void fullyRemoveQueueSynchronizer(void *input) {

    RCUSynchronizer *queueSynchronizer;

    queueSynchronizer = (RCUSynchronizer *) input;
    freeRCUSynchronizer(queueSynchronizer, &fullyRemoveQueue);
}

void fullyRemoveRBTreeSynchronizer(RCUSynchronizer *input) {

    freeRBTreeContentIncluded(input->RCUProtectedDataStructure, &fullyRemoveQueueSynchronizer);
    kfree(input);
}