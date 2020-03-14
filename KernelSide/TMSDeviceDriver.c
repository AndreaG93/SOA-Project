#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/uaccess.h>
#include <linux/kobject.h>
#include <linux/sysfs.h>

#include "BasicOperations/BasicDefines.h"
#include "DataStructure/RCURedBlackTree.h"
#include "DataStructure/SemiLockFreeQueue.h
#include "DataStructure/DataUnit.h"
#include "ModuleMetadata.h"
#include "TMSDeviceDriver.h"
#include "KObjectManagement.h"

static int majorNumber;
static RCURedBlackTree *RCUTree;
static struct kobject *kObjectParent;

static ssize_t var_show(struct kobject *kobj, struct attribute *attr, char *buf) {

    unsigned long waitFreeQueueIndex;
    WaitFreeQueue *waitFreeQueue;

    waitFreeQueueIndex = stringToLong(kobj->name);
    waitFreeQueue = search(RCUTree, waitFreeQueueIndex);

    printk("'%s': 'var_show' function is been called managing 'WaitFreeQueue' with index %d!\n", DEVICE_DRIVER_NAME,
           waitFreeQueueIndex);

    if (strcmp(attr->name, "max_message_size") == 0)
        return sprintf(buf, "%d\n", waitFreeQueue->maxMessageSize);
    else
        return sprintf(buf, "%d\n", waitFreeQueue->maxStorageSize);
}

static ssize_t var_store(struct kobject *kobj, struct attribute *attr, const char *buf, size_t count) {

    WaitFreeQueue *waitFreeQueue = search(RCUTree, 0);

    if (strcmp(attr->name, "max_message_size") == 0)
        sscanf(buf, "%du", &(waitFreeQueue->maxMessageSize));
    else
        sscanf(buf, "%du", &(waitFreeQueue->maxStorageSize));

    return count;
}

static int TMS_open(struct inode *inode, struct file *file) {

    SemiLockFreeQueue *queue;
    unsigned int queueID = iminor(inode);

    printk("'%s': 'TMS_open' function is been called with minor number %d!\n", DEVICE_DRIVER_NAME, queueID);

    queue = (SemiLockFreeQueue *) search(RCUTree, queueID)
    if (queue == NULL) {
        queue = allocateAndInitializeSemiLockFreeQueue(MAX_MESSAGE_SIZE, MAX_STORAGE_SIZE, queueID);
        insert(RCUTree, queueID, queue);
    }

    return 0;
}

static ssize_t TMS_read(struct file *file, char *userBuffer, size_t size, loff_t *offset) {

    DataUnit *dataUnit;
    SemiLockFreeQueue *queue;
    unsigned int queueID = iminor(inode);

    printk("'%s': 'TMS_read' function is been called with minor number %d!\n", DEVICE_DRIVER_NAME, queueID);

    queue = (SemiLockFreeQueue *) search(RCUTree, queueID);

    dataUnit = dequeue(queue)
    if (dataUnit != NULL) {

        if (size < dataUnit)

        copy_to_user(userBuffer, outputData, size);
        free(outputData);

    } else
        return -ENOENT
}

static ssize_t TMS_write(struct file *file, const char *buffer, size_t size, loff_t *offset) {

    DataUnit *dataUnit;
    SemiLockFreeQueue *queue;
    unsigned int queueID = iminor(inode);

    printk("'%s': 'TMS_write' function is been called with minor number %d!\n", DEVICE_DRIVER_NAME, queueID);

    queue = (SemiLockFreeQueue *) search(RCUTree, queueID);


}


static int TMS_release(struct inode *inode, struct file *file) {

    printk(KERN_DEBUG
    "'%s': 'TMS_release' function is been called!\n", DEVICE_DRIVER_NAME);

    return 0;
}


static int TMS_flush(struct file *file, fl_owner_t id) {

    SemiLockFreeQueue *oldQueue;
    SemiLockFreeQueue *newQueue;
    unsigned int queueID = iminor(inode);

    printk("'%s': 'TMS_open' function is been called with minor number %d!\n", DEVICE_DRIVER_NAME, queueID);

    oldQueue = (SemiLockFreeQueue *) search(RCUTree, queueID);

    newQueue = allocateAndInitializeSemiLockFreeQueue(oldQueue->maxMessageSize, oldQueue->maxStorageSize,
                                                      oldQueue->kObject);
    atomicallySwap(RCUTree, queueID, newQueue);

    freeSemiLockFreeQueue(oldQueue);

    return 0;
}

static long TMS_unlocked_ioctl(struct file *file, unsigned int x, unsigned long y) {
    return 1;
}

static struct file_operations TimedMsgServiceOperation = {
        read: TMS_read,
        write: TMS_write,
        open: TMS_open,
        release: TMS_release,
        unlocked_ioctl: TMS_unlocked_ioctl,
        flush: TMS_flush
};


struct sysfs_ops *allocateAndInitializeSysFileSystemOperation(void) {

    struct sysfs_ops *output;

    output = kmalloc(sizeof(struct sysfs_ops), GFP_KERNEL);
    if (output == NULL)
        printk("'%s': 'struct sysfs_ops' allocation failed!\n", MODULE_NAME);
    else {
        output->show = var_show;
        output->store = var_store;
    }

    return output;
}


void createAndInitializeNewWaitFreeQueue(unsigned long id) {

    WaitFreeQueue *waitFreeQueue;
    struct kobject *kObject;
    struct kobj_type *kType;
    const struct attribute_group *attributesGroups;
    struct sysfs_ops *sysFSOperations;
    char *kObjectName;

    kObjectName = convertIntToString(id);
    printk("'%s': creating 'WaitFreeQueue' %s!\n", MODULE_NAME, kObjectName);

    kObject = kobject_create_and_add(kObjectName, kObjectParent);
    if (kObject == NULL) {

        printk("'%s': 'kobject_create_and_add' failed!\n", MODULE_NAME);
        return;

    } else {

        kType = kmalloc(sizeof(struct kobj_type), GFP_KERNEL);
        if (kType == NULL) {

            printk("'%s': 'struct kobj_type' allocation failed!\n", MODULE_NAME);
            goto FAILURE_FREEING_K_OBJECT;

        } else {


            attributesGroups = allocateAttributeGroup(S_IWUSR | S_IRWXG, 2, "max_message_size", "max_storage_size");
            if (attributesGroups == NULL) {

                printk("'%s': 'allocateKObjectAttributeGroup' failed!\n", MODULE_NAME);

                goto FAILURE_FREEING_K_OBJECT;
            }

            sysFSOperations = allocateAndInitializeSysFileSystemOperation();
            if (sysFSOperations == NULL) {

                printk("'%s': 'allocateAndInitializeSysFileSystemOperation' failed!\n", MODULE_NAME);

                freeAttributeGroup(attributesGroups, 2);
                goto FAILURE_FREEING_K_OBJECT;
            }

            kType->default_groups = &attributesGroups;
            kType->sysfs_ops = sysFSOperations;

            kObject->ktype = kType;

            waitFreeQueue = allocateAndInitializeWaitFreeQueue(5, 5, kObject);

            insert(RCUTree, id, waitFreeQueue);

            createAttributeGroupSysFiles(kObject, 2);
        }
    }

    FAILURE_FREEING_K_OBJECT:

    kobject_put(kObject);
    kfree(kObject);
}


int registerTMSDeviceDriver(void) {

    majorNumber = register_chrdev(0, DEVICE_DRIVER_NAME, &TimedMsgServiceOperation);
    if (majorNumber < 0) {
        printk("'%s': char device registration failed!\n", MODULE_NAME);
        return FAILURE;
    } else {
        printk("'%s' char device is been successfully registered with major number %d!\n", MODULE_NAME, majorNumber);

        RCUTree = kmalloc(sizeof(RCUTree), GFP_KERNEL);
        RCUTree->rb_node = NULL;

        //kObjectParent = kobject_create_and_add("TSM", kernel_kobj);

        //createAndInitializeNewWaitFreeQueue(0);
        //createAndInitializeNewWaitFreeQueue(1);

        return SUCCESS;
    }
}

void unregisterTMSDeviceDriver(void) {

    //"mknod /dev/TMS c 238 0"
    // rm -rF /dev/TMS
    //cdev_del(charDeviceFile);

    unregister_chrdev(majorNumber, DEVICE_DRIVER_NAME);
    printk("'%s': char device is been successfully unregistered!!\n", MODULE_NAME);

    //kobject_put(kObjectParent);
}