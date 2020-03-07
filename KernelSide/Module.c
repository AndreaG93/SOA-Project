#include <linux/module.h>
#include <linux/kernel.h>

#define MODULE_NAME "Timed-Messaging-System"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Andrea Graziani <andrea.graziani93@outlook.it>");
MODULE_VERSION("1.0a");
MODULE_DESCRIPTION("A simple system service that allows exchanging messages across threads.");

int init_module() {

    printk(KERN_NOTICE "'%s' module successfully installed!\n", MODULE_NAME);
    return 0;
}


void cleanup_module() {
    printk(KERN_NOTICE "'%s' module successfully removed!\n", MODULE_NAME);
}