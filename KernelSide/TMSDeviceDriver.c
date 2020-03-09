#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/uaccess.h>

#include "RCURedBlackTree.h"
#include "./Common.h"
#include "./TMSDeviceDriver.h"

static int majorNumber;
//static struct cdev *charDeviceFile;
//static dev_t devNo;

static char *kernelBuffer;

static unsigned int max_message_size = 8;

RCURedBlackTree *RCUTree;















static int TMS_open(struct inode *inode, struct file *file) {

    printk(KERN_DEBUG
    "'%s': 'TMS_open' function is been called!\n", DEVICE_DRIVER_NAME);

    //unsigned int mdsa = iminor(file->f_inode);
    //unsigned int mdsa = iminor(inode);

    printk(KERN_DEBUG
    "'%s': 'TMS_open' function is been called on minor number %d   e %d!\n", DEVICE_DRIVER_NAME, iminor(inode), iminor(file->f_inode));


    return 0;
}

static ssize_t TMS_read(struct file *file, char *userBuffer, size_t size, loff_t *offset) {

    unsigned long bytesNotCopied;

    void *data = search(RCUTree, iminor(file->f_inode));
    if (data != NULL)
        bytesNotCopied = copy_to_user(userBuffer, data, 6);

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



int registerTMSDeviceDriver(void) {

    majorNumber = register_chrdev(0, DEVICE_DRIVER_NAME, &TimedMsgServiceOperation);
    if (majorNumber < 0) {
        printk(KERN_WARNING
        "'%s' char device registration failed!\n", DEVICE_DRIVER_NAME);
        return FAILURE;
    } else {
        printk(KERN_NOTICE
        "'%s' char device is been successfully registered with major number %d!\n", DEVICE_DRIVER_NAME, majorNumber);




        /*
        charDeviceFile = cdev_alloc();
        cdev_init(charDeviceFile, &TimedMsgServiceOperation);


        charDeviceFile->ops = &TimedMsgServiceOperation;
        charDeviceFile->owner = THIS_MODULE;

        devNo = MKDEV(majorNumber, 0);
        int error = cdev_add(charDeviceFile, devNo, 0);
        if(error) {
            printk(KERN_WARNING "Error adding device");
        }
*/

        kernelBuffer = kmalloc(6* sizeof(char), GFP_KERNEL);
        if (kernelBuffer == NULL) {
            printk(KERN_WARNING
            "'%s' allocation failed!\n", DEVICE_DRIVER_NAME);
        }

        strcpy(kernelBuffer, "Andrea");

        RCUTree = allocateRCURedBlackTree();

        insert(RCUTree, 0, kernelBuffer);
        insert(RCUTree, 1, kernelBuffer);

        return SUCCESS;
    }
}

void unregisterTMSDeviceDriver(void) {

    //"mknod /dev/TMS c 238 0"
    // rm -rF /dev/TMS
    //cdev_del(charDeviceFile);

    unregister_chrdev(majorNumber, DEVICE_DRIVER_NAME);
    printk(KERN_NOTICE
    "'%s' char device is been successfully unregistered!\n", DEVICE_DRIVER_NAME);

    if (kernelBuffer)
        kfree(kernelBuffer);

}