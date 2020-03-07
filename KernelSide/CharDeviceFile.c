#include <linux/kernel.h>
#include <linux/slab.h>


#define DEVICE_NAME "TimedMessagingSystem"




int TMS_open(struct inode *, struct file *) {
    return 1;
}

ssize_t TMS_read(struct file *, char *, size_t, loff_t *) {
    return 1;
}

ssize_t TMS_write(struct file *, const char *, size_t, loff_t *) {
    return 1;
}

int isCharDeviceRegistrationSuccessful() {

    struct file_operations fops = {
            read: NULL,
            write: NULL,
            open: NULL,
            release: NULL
    };

    Major = register_chrdev(0, DEVICE_NAME, &fops);
    if (Major < 0) {
        printk(KERN_ALERT "Registering char device failed with %d\n", Major);
        return Major;
    }


}