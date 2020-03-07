#include <linux/module.h>
#include <linux/kernel.h>
#include "./Common.h"
#include "./TMSDeviceDriver.h"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Andrea Graziani <andrea.graziani93@outlook.it>");
MODULE_VERSION("1.0a");
MODULE_DESCRIPTION("A simple system service that allows exchanging messages across threads.");

int init_module() {

    registerTMSDeviceDriver();
    printk(KERN_NOTICE "'%s' module successfully installed!\n", TMS_MODULE_NAME);
    return 0;
}


void cleanup_module() {
    unregisterTMSDeviceDriver();
    printk(KERN_NOTICE "'%s' module successfully removed!\n", TMS_MODULE_NAME);
}