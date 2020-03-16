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

struct kobject *
allocateSemiLockFreeQueueKObject(unsigned long queueID, struct kobject *kObjectParent,
                                 ssize_t (*show)(struct kobject *, struct kobj_attribute *, char *),
                                 ssize_t (*store)(struct kobject *, struct kobj_attribute *, const char *, size_t)) {

    struct kobject *kObject;

    struct attribute_group *attributeGroup;
    struct attribute **attributes;

    struct kobj_attribute* kObjectAttribute1;
    struct kobj_attribute* kObjectAttribute2;

    kObjectAttribute1 = kmalloc(sizeof(struct kobj_attribute), GFP_KERNEL);
    if (kObjectAttribute1 == NULL)
        return NULL;

    kObjectAttribute2 = kmalloc(sizeof(struct kobj_attribute), GFP_KERNEL);
    if (kObjectAttribute2 == NULL) {

        kfree(kObjectAttribute1);

        return NULL;
    }

    kObjectAttribute1->attr.name = "max_message_size";
    kObjectAttribute1->attr.mode = 0644;
    kObjectAttribute1->show = show;
    kObjectAttribute1->store = store;

    kObjectAttribute2->attr.name = "max_storage_size";
    kObjectAttribute2->attr.mode = 0644;
    kObjectAttribute2->show = show;
    kObjectAttribute2->store = store;

    attributes = kmalloc(sizeof(struct attribute *) * 3, GFP_KERNEL);
    if (attributes == NULL) {

        kfree(kObjectAttribute1);
        kfree(kObjectAttribute2);

        return NULL;
    }


    attributes[0] = &(kObjectAttribute1->attr);
    attributes[1] = &(kObjectAttribute2->attr);
    attributes[2] = NULL;

    attributeGroup = kmalloc(sizeof(struct attribute_group), GFP_KERNEL);
    if (attributeGroup == NULL) {

        kfree(kObjectAttribute1);
        kfree(kObjectAttribute2);
        kfree(attributes);
        return NULL;
    }

    attributeGroup->attrs = attributes;
    attributeGroup->name = "SemiLockFreeQueue";

    kObject = kobject_create_and_add(convertIntToString(queueID), kObjectParent);
    if (kObject == NULL) {

        kfree(attributeGroup);
        kfree(attributes);
        kfree(kObjectAttribute1);
        kfree(kObjectAttribute2);

        return NULL;
    }

    if (sysfs_create_group(kObject, attributeGroup) != 0) {

        kobject_put(kObject);
        return NULL;
    }

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
allocateNewQueueRCUSynchronizer(unsigned long queueID, struct kobject *kObjectParent,
                                ssize_t (*show)(struct kobject *, struct kobj_attribute *, char *),
                                ssize_t (*store)(struct kobject *, struct kobj_attribute *, const char *, size_t)) {

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

    kobject_put(queue->kObject);
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