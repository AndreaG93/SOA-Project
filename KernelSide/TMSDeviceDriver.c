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

ssize_t TMS_write(struct file *, const char *buffer, size_t size, loff_t *offset) {
    return 1;
}

int TMS_release(struct inode *inode, struct file *file) {
    return 1;
}

int TMS_flush(struct file *file) {
    return 1;
}

int TMS_ioctl(struct inode *inode, struct file *file, unsigned int x, unsigned long y) {
    return 1;
}

static int majorNumber;

int registerTMSDeviceDriver(void) {

    struct file_operations TimedMsgServiceOperation = {
            read: TMS_read,
            write: TMS_write,
            open: TMS_open,
            release: TMS_release,
            ioctl: TMS_ioctl,
            flush: TMS_flush
    };

    majorNumber = register_chrdev(0, TMS_CHAR_DEVICE_NAME, &TimedMsgServiceOperation);
    if (majorNumber < 0) {
        printk(KERN_ALERT
        "'%s' char device registration failed!\n", TMS_CHAR_DEVICE_NAME);
        return FAILURE;
    } else {
        printk(KERN_NOTICE
        "'%s' char device is been successfully registered with major number %d!\n", TMS_CHAR_DEVICE_NAME, majorNumber);
        return SUCCESS;
    }
}

void unregisterTMSDeviceDriver(void) {

    unregister_chrdev(majorNumber, TMS_CHAR_DEVICE_NAME);
    printk(KERN_NOTICE
    "'%s' char device is been successfully unregistered!\n", TMS_CHAR_DEVICE_NAME);
}