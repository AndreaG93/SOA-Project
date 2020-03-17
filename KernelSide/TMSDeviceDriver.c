#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/uaccess.h>
#include <linux/kobject.h>
#include <linux/sysfs.h>
#include <linux/workqueue.h>

#include "Common/BasicDefines.h"
#include "Common/ModuleMetadata.h"
#include "Common/BasicOperations.h"

#include "DataStructure/RBTree.h"
#include "DataStructure/SemiLockFreeQueue.h"
#include "DataStructure/RCUSynchronizer.h"
#include "DataStructure/Message.h"
#include "DataStructure/Session.h"

#include "ModuleFunctions.h"
#include "MessagesManagement.h"

static RCUSynchronizer *RBTreeSynchronizer;
static int majorNumber;
static struct kobject *kObjectParent;

//struct work_struct work;
//struct delayed_work delayedWork;

static ssize_t TMS_show(struct kobject *kobj, struct kobj_attribute *kObjAttribute, char *buf) {

    SemiLockFreeQueue *queue;
    RCUSynchronizer *queueSynchronizer;
    ssize_t output;
    int queueID;
    unsigned int epoch;

    queueID = stringToLong(kobj->name);

    printk("'%s': 'TMS_show' function is been called managing 'SemiLockFreeQueue' with index %d!\n", MODULE_NAME,
           queueID);

    queueSynchronizer = getQueueRCUSynchronizer(RBTreeSynchronizer, queueID);

    epoch = readLockRCUGettingEpoch(queueSynchronizer);

    queue = (SemiLockFreeQueue *) queueSynchronizer->RCUProtectedDataStructure;

    if (strcmp(kObjAttribute->attr.name, "max_message_size") == 0) {
        output = sprintf(buf, "%ld\n", queue->maxMessageSize);
    } else {
        output = sprintf(buf, "%ld\n", queue->maxStorageSize);
    }

    readUnlockRCU(queueSynchronizer, epoch);

    return output;
}

static ssize_t TMS_store(struct kobject *kobj, struct kobj_attribute *kObjAttribute, const char *buf, size_t count) {

    SemiLockFreeQueue *queue;
    RCUSynchronizer *queueSynchronizer;
    int queueID;

    queueID = stringToLong(kobj->name);

    printk("'%s': 'TMS_store' function is been called managing 'SemiLockFreeQueue' with index %d!\n", MODULE_NAME,
           queueID);

    queueSynchronizer = getQueueRCUSynchronizer(RBTreeSynchronizer, queueID);

    writeLockRCU(queueSynchronizer);

    queue = (SemiLockFreeQueue *) queueSynchronizer->RCUProtectedDataStructure;

    if (strcmp(kObjAttribute->attr.name, "max_message_size") == 0) {
        sscanf(buf, "%ldu", &(queue->maxMessageSize));
    } else {
        sscanf(buf, "%ldu", &(queue->maxStorageSize));
    }

    printk("'%s': 'maxMessageSize' variable of 'SemiLockFreeQueue' with index %d is now %ld!\n", MODULE_NAME, queueID,
           queue->maxMessageSize);
    printk("'%s': 'maxStorageSize' variable of 'SemiLockFreeQueue' with index %d is now %ld!\n", MODULE_NAME, queueID,
           queue->maxStorageSize);

    writeUnlockRCU(queueSynchronizer, queueSynchronizer->RCUProtectedDataStructure);

    return count;
}

static int TMS_open(struct inode *inode, struct file *file) {

    RCUSynchronizer *queueSynchronizer;
    int queueID;

    queueID = iminor(file->f_inode);

    printk("'%s': 'TMS_open' function is been called with minor number %d!\n", MODULE_NAME, queueID);

    writeLockRCU(RBTreeSynchronizer);

    queueSynchronizer = (RCUSynchronizer *) searchRBTree(RBTreeSynchronizer->RCUProtectedDataStructure, queueID);
    if (queueSynchronizer == NULL) {

        RBTree *newRBTree;
        RBTree *oldRBTree;

        queueSynchronizer = allocateNewQueueRCUSynchronizer(queueID, kObjectParent, &TMS_show, &TMS_store);

        oldRBTree = RBTreeSynchronizer->RCUProtectedDataStructure;
        newRBTree = copyRBTree(oldRBTree);

        insertRBTree(newRBTree, queueID, queueSynchronizer);

        writeUnlockRCU(RBTreeSynchronizer, newRBTree);

        freeRBTreeContentExcluded(oldRBTree);

    } else
        writeUnlockRCU(RBTreeSynchronizer, RBTreeSynchronizer->RCUProtectedDataStructure);

    file->private_data = allocateSession(queueSynchronizer);

    return SUCCESS;
}

static ssize_t TMS_read(struct file *file, char *userBuffer, size_t userBufferSize, loff_t *offset) {

    Session *session;

    session = (Session *) file->private_data;

    if (session->dequeueDelay > 0) {

        printk("'%s': 'TMS_read' function is been called with 'SET_RECV_TIMEOUT' command (%lu)!\n", MODULE_NAME,
               session->dequeueDelay);
        return SUCCESS;

    } else {

        printk("'%s': 'TMS_read' function is been called!\n", MODULE_NAME);
        return dequeueMessage(session->queueSynchronizer, userBuffer, userBufferSize);
    }
}

static ssize_t TMS_write(struct file *file, const char *userBuffer, size_t userBufferSize, loff_t *offset) {

    Session *session;

    session = (Session *) file->private_data;

    if (session->enqueueDelay > 0) {

        printk("'%s': 'TMS_write' function is been called with 'SET_SEND_TIMEOUT' command (%lu)!\n", MODULE_NAME,
               session->enqueueDelay);
        return SUCCESS;

    } else {

        printk("'%s': 'TMS_write' function is been called!\n", MODULE_NAME);
        return enqueueMessage(session->queueSynchronizer, userBuffer, userBufferSize);
    }
}


static int TMS_release(struct inode *inode, struct file *file) {

    printk("'%s': 'TMS_release' function is been called!\n", MODULE_NAME);

    freeSession(file->private_data);
    file->private_data = NULL;

    return 0;
}


static int TMS_flush(struct file *file, fl_owner_t id) {

    /*
    SemiLockFreeQueue *oldQueue;
    SemiLockFreeQueue *newQueue;
    RCUSynchronizer *queueSynchronizer;
    int queueID;

    queueID = iminor(file->f_inode);

    printk("'%s': 'TMS_flush' function is been called with minor number %d!\n", MODULE_NAME, queueID);

    queueSynchronizer = getQueueRCUSynchronizer(RBTreeSynchronizer, queueID);

    writeLockRCU(queueSynchronizer);

    oldQueue = queueSynchronizer->RCUProtectedDataStructure;
    newQueue = allocateSemiLockFreeQueue(oldQueue->maxMessageSize, oldQueue->maxStorageSize, oldQueue->kObject);

    writeUnlockRCU(queueSynchronizer, newQueue);

    freeSemiLockFreeQueue(oldQueue, &fullyRemoveMessage);
*/
    return SUCCESS;
}

static long TMS_unlocked_ioctl(struct file *file, unsigned int command, unsigned long parameter) {

    Session *session;

    printk("'%s': 'TMS_unlocked_ioctl' function is been called!\n", MODULE_NAME);

    session = (Session *) file->private_data;

    switch (command) {

        case SET_SEND_TIMEOUT:
            printk("'%s': 'SET_SEND_TIMEOUT' command received with parameter: %ld\n", MODULE_NAME, parameter);
            session->enqueueDelay = parameter;
            break;

        case SET_RECV_TIMEOUT:
            printk("'%s': 'SET_RECV_TIMEOUT' command received with parameter: %ld\n", MODULE_NAME, parameter);
            session->dequeueDelay = parameter;
            break;

        case REVOKE_DELAYED_MESSAGES:
            printk("'%s': 'REVOKE_DELAYED_MESSAGES' command received!\n", MODULE_NAME);
            //revokeDelayedMessages(session);
            break;

        default:
            return FAILURE;
    }

    return SUCCESS;
}

static struct file_operations TMSOperation = {
        read: TMS_read,
        write: TMS_write,
        open: TMS_open,
        release: TMS_release,
        unlocked_ioctl: TMS_unlocked_ioctl,
        flush: TMS_flush
};

/*
static void example(struct work_struct *input) {
    printk("'%s': WORK QUEUE!\n", MODULE_NAME);
}
*/

int registerTMSDeviceDriver(void) {

    majorNumber = register_chrdev(0, CHAR_DEVICE_NAME, &TMSOperation);
    if (majorNumber < 0) {

        printk("'%s': char device registration failed!\n", MODULE_NAME);
        return FAILURE;

    } else {

        RBTree *rbTree = allocateRBTree();
        if (rbTree == NULL) {

            printk("'%s': 'rbTree' allocation failed!\n", MODULE_NAME);
            return FAILURE;
        }

        RBTreeSynchronizer = allocateRCUSynchronizer(rbTree);
        if (RBTreeSynchronizer == NULL) {

            printk("'%s': 'RBTreeSynchronizer' allocation failed!\n", MODULE_NAME);
            kfree(rbTree);
            return FAILURE;
        }

        kObjectParent = kobject_create_and_add("TSM", kernel_kobj);

        printk("'%s': char device is been successfully registered with major number %d!\n", MODULE_NAME, majorNumber);


        /*
        INIT_WORK(&work, example);

        INIT_DELAYED_WORK(&delayedWork, example);


        schedule_work(&work);
        schedule_delayed_work(&delayedWork, 2500);
        cancel_delayed_work_sync(&delayedWork);
*/

        return SUCCESS;
    }
}

void unregisterTMSDeviceDriver(void) {

    fullyRemoveRBTreeSynchronizer(RBTreeSynchronizer);

    unregister_chrdev(majorNumber, CHAR_DEVICE_NAME);
    printk("'%s': char device is been successfully unregistered!!\n", MODULE_NAME);

    kobject_put(kObjectParent);
}
