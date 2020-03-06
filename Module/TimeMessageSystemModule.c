//
// Created by andrea on 06/03/20.
//

#include "TimeMessageSystemModule.h"

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <stddef.h>

static struct file_operations fops = {
        .write = NULL,
        .read = NULL,
        .open =  NULL,
        .release = NULL
};

int init_module(void)
{



    int Major = register_chrdev(0, DEVICE_NAME, &fops);

    if (Major < 0) {
        printk(KERN_ALERT "Registering char device failed with %d\n", Major);
        return Major;
    }

    printk(KERN_INFO "I was assigned major number %d. To talk to\n", Major);
    printk(KERN_INFO "the driver, create a dev file with\n");
    printk(KERN_INFO "'mknod /dev/%s c %d 0'.\n", DEVICE_NAME, Major);
    printk(KERN_INFO "Try various minor numbers. Try to cat and echo to\n");
    printk(KERN_INFO "the device file.\n");
    printk(KERN_INFO "Remove the device file and module when done.\n");

    return SUCCESS;
}