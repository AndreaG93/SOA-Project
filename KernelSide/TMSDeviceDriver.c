#include "RCURedBlackTree.h"
#include "ModuleInformations.h"
#include "KernelLogManagement.h"
#include "TMSDeviceDriver.h"

#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/uaccess.h>
#include <linux/kobject.h>
#include <linux/sysfs.h>.

static int majorNumber;
static RCURedBlackTree *RCUTree;
static kobject *kObjectParent;

static ssize_t var_show(struct kobject *kobj, struct attribute *attr, char *buf) {

    WaitFreeQueue *waitFreeQueue = search(RCUTree, 0);

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

    printk("'%s': 'TMS_open' function is been called!\n", DEVICE_DRIVER_NAME);

    //unsigned int mdsa = iminor(file->f_inode);
    //unsigned int mdsa = iminor(inode);

    printk(KERN_DEBUG
    "'%s': 'TMS_open' function is been called on minor number %d   e %d!\n", DEVICE_DRIVER_NAME, iminor(inode), iminor(
            file->f_inode));


    return 0;
}

static ssize_t TMS_read(struct file *file, char *userBuffer, size_t size, loff_t *offset) {

    printk(KERN_DEBUG
    "'%s': 'TMS_read' function is been called on device file (%d %d)!\n", DEVICE_DRIVER_NAME, imajor(
            file->f_inode), iminor(file->f_inode));

    unsigned long bytesNotCopied;

    void *data = search(RCUTree, 0);
    if (data != NULL) {
        bytesNotCopied = copy_to_user(userBuffer, data, 6);
    }

    return 0;
}

static ssize_t TMS_write(struct file *file, const char *buffer, size_t size, loff_t *offset) {
    return 1;
}


static int TMS_release(struct inode *inode, struct file *file) {

    printk(KERN_DEBUG
    "'%s': 'TMS_release' function is been called!\n", DEVICE_DRIVER_NAME);

    return 0;
}


static int TMS_flush(struct file *file, fl_owner_t id) {

    printk(KERN_DEBUG
    "'%s': 'TMS_flush' function is been called!\n", DEVICE_DRIVER_NAME);

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
        printWarningMessageToKernelLog("'struct sysfs_ops' allocation failed!");
    else {
        output->show = var_show;
        output->store = var_store;
    }

    return output;
}

void createAndInitializeNewWaitFreeQueue(unsigned long id) {

    WaitFreeQueue *waitFreeQueue;
    struct kObject *kObject;
    struct kobj_type *kType;
    struct attribute_group **attributesGroups;
    struct sysfs_ops *sysFSOperations;

    kObject = kobject_create_and_add("0", kObjectParent);
    if (currentKObject == NULL) {

        printWarningMessageToKernelLog("'kobject_create_and_add' failed!");
        return;

    } else {

        ktype = kmalloc(sizeof(struct kobj_type), GFP_KERNEL);
        if (ktype == NULL) {

            printWarningMessageToKernelLog("'struct kobj_type' allocation failed!");
            goto FAILURE_FREEING_K_OBJECT;

        } else {

            attribute_group = &allocateKObjectAttributeGroup(2, S_IWUSR | S_IRWXG, "max_message_size",
                                                             "max_storage_size");
            if (attribute_group == NULL) {

                printWarningMessageToKernelLog("'allocateKObjectAttributeGroup' failed!");

                goto FAILURE_FREEING_K_OBJECT;
            }

            sysFSOperations = allocateAndInitializeSysFileSystemOperation();
            if (attribute_group == NULL) {

                printWarningMessageToKernelLog("'allocateSysFileSystemOperation' failed!");

                freeAttributeGroup(attribute_group[0], 2);
                goto FAILURE_FREEING_K_OBJECT;
            }

            ktype->default_groups = attribute_group;
            ktype->sysfs_ops = sysFSOperations;

            waitFreeQueue = allocateAndInitializeWaitFreeQueue(5, 5, kObject);

            insert(RCUTree, id, waitFreeQueue)

            createAttributeGroupSysFiles();
        }
    }

    FAILURE_FREEING_K_OBJECT:

    kobject_put(kObject);
    kfree(kObject)
}


int registerTMSDeviceDriver(void) {

    majorNumber = register_chrdev(0, DEVICE_DRIVER_NAME, &TimedMsgServiceOperation);
    if (majorNumber < 0) {
        printWarningMessageToKernelLog("char device registration failed! ");
        return FAILURE;
    } else {
        printk("'%s' char device is been successfully registered with major number %d!\n", MODULE_NAME, majorNumber);

        RCUTree = kmalloc(sizeof(RCUTree), GFP_KERNEL);
        RCUTree->rb_node = NULL;

        kObjectParent = kobject_create_and_add("TSM", kernel_kobj);

        createAndInitializeNewWaitFreeQueue(0);

        return SUCCESS;
    }
}

void unregisterTMSDeviceDriver(void) {

    //"mknod /dev/TMS c 238 0"
    // rm -rF /dev/TMS
    //cdev_del(charDeviceFile);

    unregister_chrdev(majorNumber, DEVICE_DRIVER_NAME);
    printMessageToKernelLog("char device is been successfully unregistered!\n")

    kobject_put(kObjectParent);
}