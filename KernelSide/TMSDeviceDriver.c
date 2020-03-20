#include <linux/kernel.h>

#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/uaccess.h>
#include <linux/kobject.h>
#include <linux/sysfs.h>
#include <linux/workqueue.h>
#include <linux/wait.h>
#include <linux/sched.h>

#include "Common/BasicDefines.h"
#include "Common/ModuleMetadata.h"
#include "Common/BasicOperations.h"

#include "DataStructure/RBTree.h"
#include "DataStructure/SemiLockFreeQueue.h"
#include "DataStructure/RCUSynchronizer.h"
#include "DataStructure/Message.h"
#include "DataStructure/Session.h"
#include "DataStructure/KObjectManagement.h"
#include "DataStructure/DeviceFileInstance.h"

#include "DelayedOperations.h"
#include "CleaningFunctions.h"
#include "MessagesManagement.h"


static RCUSynchronizer *DeviceFileInstanceRBTreeSynchronizer;
static unsigned int majorNumber;
static struct kobject *kObjectParent;

static unsigned long currentActiveSessions;
static unsigned char isModuleActive;

static ssize_t TMS_show(struct kobject *kobj, struct kobj_attribute *kObjAttribute, char *buf) {

    SemiLockFreeQueue *semiLockFreeQueue;
    RCUSynchronizer *semiLockFreeQueueSynchronizer;
    DeviceFileInstance *deviceFileInstance;
    unsigned int minorDeviceNumber;
    unsigned int epoch;
    ssize_t output;

    minorDeviceNumber = stringToLong(kobj->name);

    printk("'%s': 'TMS_show' function is been called! 'SemiLockFreeQueue' ID %d!\n", MODULE_NAME, minorDeviceNumber);

    deviceFileInstance = getDeviceFileInstanceFromSynchronizer(DeviceFileInstanceRBTreeSynchronizer, minorDeviceNumber);

    semiLockFreeQueueSynchronizer = deviceFileInstance->semiLockFreeQueueRCUSynchronizer;

    epoch = readLockRCUGettingEpoch(semiLockFreeQueueSynchronizer);

    semiLockFreeQueue = (SemiLockFreeQueue *) semiLockFreeQueueSynchronizer->RCUProtectedDataStructure;

    if (strcmp(kObjAttribute->attr.name, "max_message_size") == 0) {
        output = sprintf(buf, "%ld\n", semiLockFreeQueue->maxMessageSize);
    } else {
        output = sprintf(buf, "%ld\n", semiLockFreeQueue->maxStorageSize);
    }

    readUnlockRCU(semiLockFreeQueueSynchronizer, epoch);

    return output;
}

static ssize_t TMS_store(struct kobject *kobj, struct kobj_attribute *kObjAttribute, const char *buf, size_t count) {

    SemiLockFreeQueue *semiLockFreeQueue;
    RCUSynchronizer *semiLockFreeQueueSynchronizer;
    DeviceFileInstance *deviceFileInstance;
    unsigned int minorDeviceNumber;

    minorDeviceNumber = stringToLong(kobj->name);

    printk("'%s': 'TMS_store' function is been called! 'SemiLockFreeQueue' ID %d!\n", MODULE_NAME, minorDeviceNumber);

    deviceFileInstance = getDeviceFileInstanceFromSynchronizer(DeviceFileInstanceRBTreeSynchronizer, minorDeviceNumber);

    semiLockFreeQueueSynchronizer = deviceFileInstance->semiLockFreeQueueRCUSynchronizer;

    writeLockRCU(semiLockFreeQueueSynchronizer);

    semiLockFreeQueue = (SemiLockFreeQueue *) semiLockFreeQueueSynchronizer->RCUProtectedDataStructure;

    if (strcmp(kObjAttribute->attr.name, "max_message_size") == 0) {
        sscanf(buf, "%ldu", &(semiLockFreeQueue->maxMessageSize));
    } else {
        sscanf(buf, "%ldu", &(semiLockFreeQueue->maxStorageSize));
    }

    printk("'%s': 'maxMessageSize' variable of 'SemiLockFreeQueue' with index %d is now %ld!\n", MODULE_NAME,
           minorDeviceNumber, semiLockFreeQueue->maxMessageSize);
    printk("'%s': 'maxStorageSize' variable of 'SemiLockFreeQueue' with index %d is now %ld!\n", MODULE_NAME,
           minorDeviceNumber, semiLockFreeQueue->maxStorageSize);

    writeUnlockRCU(semiLockFreeQueueSynchronizer, semiLockFreeQueueSynchronizer->RCUProtectedDataStructure);

    return count;
}

static int TMS_open(struct inode *inode, struct file *file) {

    DeviceFileInstance *deviceFileInstance;
    unsigned int minorDeviceNumber;

    if (!isModuleActive)
        return FAILURE;
    else
        __sync_add_and_fetch(&currentActiveSessions, 1);

    minorDeviceNumber = iminor(file->f_inode);

    printk("'%s': 'TMS_open' function is been called with minor number %d!\n", MODULE_NAME, minorDeviceNumber);

    deviceFileInstance = getDeviceFileInstanceFromSynchronizer(DeviceFileInstanceRBTreeSynchronizer, minorDeviceNumber);

    if (deviceFileInstance == NULL) {

        writeLockRCU(DeviceFileInstanceRBTreeSynchronizer);

        deviceFileInstance = searchRBTree(DeviceFileInstanceRBTreeSynchronizer->RCUProtectedDataStructure,
                                          minorDeviceNumber);

        if (deviceFileInstance != NULL)
            writeUnlockRCU(DeviceFileInstanceRBTreeSynchronizer,
                           DeviceFileInstanceRBTreeSynchronizer->RCUProtectedDataStructure);
        else {

            RBTree *newRBTree;
            RBTree *oldRBTree;

            deviceFileInstance = allocateDeviceFileInstance(minorDeviceNumber, kObjectParent, &TMS_show, &TMS_store);
            if (deviceFileInstance == NULL) {

                writeUnlockRCU(DeviceFileInstanceRBTreeSynchronizer,
                               DeviceFileInstanceRBTreeSynchronizer->RCUProtectedDataStructure);
                return ERR_ALLOCATION_FAILED;

            } else {

                oldRBTree = DeviceFileInstanceRBTreeSynchronizer->RCUProtectedDataStructure;
                newRBTree = copyRBTree(oldRBTree);

                insertRBTree(newRBTree, minorDeviceNumber, deviceFileInstance);

                writeUnlockRCU(DeviceFileInstanceRBTreeSynchronizer, newRBTree);

                freeRBTreeContentExcluded(oldRBTree);
            }
        }
    }

    file->private_data = allocateSession(deviceFileInstance->semiLockFreeQueueRCUSynchronizer);
    registerSessionIntoDeviceFileInstance(deviceFileInstance, file->private_data);

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
    DelayedEnqueueOperation *operation;

    session = (Session *) file->private_data;

    if (session->enqueueDelay > 0) {

        printk("'%s': 'TMS_write' function is been called with 'SET_SEND_TIMEOUT' command (%lu)!\n", MODULE_NAME,
               session->enqueueDelay);

        operation = allocateDelayedEnqueueOperation(session, userBuffer, userBufferSize);

        if (operation != NULL) {

            registerAndEnableDelayedEnqueueOperation(operation, &performDelayedMessageEnqueue);
            return SUCCESS;

        } else
            return ERR_ALLOCATION_FAILED;

    } else {

        printk("'%s': 'TMS_write' function is been called!\n", MODULE_NAME);
        return enqueueMessage(session->queueSynchronizer, userBuffer, userBufferSize);
    }
}

static int TMS_flush(struct file *file, fl_owner_t id) {

    DeviceFileInstance *deviceFileInstance;
    unsigned int minorDeviceNumber;

    minorDeviceNumber = iminor(file->f_inode);

    printk("'%s': 'TMS_flush' function is been called with minor number %d!\n", MODULE_NAME, minorDeviceNumber);

    deviceFileInstance = getDeviceFileInstanceFromSynchronizer(DeviceFileInstanceRBTreeSynchronizer, minorDeviceNumber);


    spin_lock(&deviceFileInstance->activeSessionsSpinlock);
    performFunctionRBTree(deviceFileInstance->activeSessions, &revokeAllDelayedEnqueueOperationsVoid);
    spin_unlock(&deviceFileInstance->activeSessionsSpinlock);

    return SUCCESS;
}

static int TMS_release(struct inode *inode, struct file *file) {

    DeviceFileInstance *deviceFileInstance;
    unsigned int minorDeviceNumber;

    minorDeviceNumber = iminor(file->f_inode);

    printk("'%s': 'TMS_release' function is been called with minor number %d!\n", MODULE_NAME, minorDeviceNumber);

    deviceFileInstance = getDeviceFileInstanceFromSynchronizer(DeviceFileInstanceRBTreeSynchronizer, minorDeviceNumber);

    unregisterSessionFromDeviceFileInstance(deviceFileInstance, file->private_data);

    freeSession(file->private_data);
    file->private_data = NULL;

    __sync_sub_and_fetch(&currentActiveSessions, 1);

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
            revokeAllDelayedEnqueueOperations(session);
            break;

        case CLEAN_QUEUE:
            printk("'%s': 'CLEAN_QUEUE' command received!\n", MODULE_NAME);
            cleanQueue(session);
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

        DeviceFileInstanceRBTreeSynchronizer = allocateRCUSynchronizer(rbTree);
        if (DeviceFileInstanceRBTreeSynchronizer == NULL) {

            printk("'%s': 'DeviceFileInstanceRBTreeSynchronizer' allocation failed!\n", MODULE_NAME);
            kfree(rbTree);
            return FAILURE;
        }

        kObjectParent = kobject_create_and_add("TSM", kernel_kobj);
        currentActiveSessions = 0;
        isModuleActive = TRUE;

        printk("'%s': char device is been successfully registered with major number %d!\n", MODULE_NAME, majorNumber);

        return SUCCESS;
    }
}

void unregisterTMSDeviceDriver(void) {

    isModuleActive = FALSE;

    asm volatile("mfence");

    while (currentActiveSessions > 0);

    fullRemoveDeviceFileInstanceRBTreeSynchronizer(DeviceFileInstanceRBTreeSynchronizer);

    unregister_chrdev(majorNumber, CHAR_DEVICE_NAME);
    printk("'%s': char device is been successfully unregistered!!\n", MODULE_NAME);

    kobject_put(kObjectParent);
}
