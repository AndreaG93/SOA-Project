#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/fs.h>
#include "./Common.h"
#include "./TMSDeviceDriver.h"



int TMS_open(struct inode *inode, struct file *file) {
    return 1;
}

ssize_t TMS_read(struct file *file, char *buffer, size_t size, loff_t *offset) {
    return 1;
}

ssize_t TMS_write(struct file *file, const char *buffer, size_t size, loff_t *offset) {
    return 1;
}

int TMS_release(struct inode *inode, struct file *file) {
    return 1;
}

int TMS_flush(struct file *file, fl_owner_t id) {
    return 1;
}

long TMS_unlocked_ioctl(struct file *file, unsigned int x, unsigned long y) {
    return 1;
}

static int majorNumber;




int registerTMSDeviceDriver(void) {

    struct file_operations TimedMsgServiceOperation = {
            read: TMS_read,
            write: TMS_write,
            open: TMS_open,
            release: TMS_release,
            unlocked_ioctl: TMS_unlocked_ioctl,
            flush: TMS_flush
    };

    majorNumber = register_chrdev(0, DEVICE_DRIVER_NAME, &TimedMsgServiceOperation);
    if (majorNumber < 0) {
        printk(KERN_WARNING
        "'%s' char device registration failed!\n", DEVICE_DRIVER_NAME);
        return FAILURE;
    } else {
        printk(KERN_NOTICE
        "'%s' char device is been successfully registered with major number %d!\n", DEVICE_DRIVER_NAME, majorNumber);
        return SUCCESS;
    }
}

void unregisterTMSDeviceDriver(void) {

    unregister_chrdev(majorNumber, DEVICE_DRIVER_NAME);
    printk(KERN_NOTICE
    "'%s' char device is been successfully unregistered!\n", DEVICE_DRIVER_NAME);
}