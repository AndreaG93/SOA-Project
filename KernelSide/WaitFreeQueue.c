#include <linux/slab.h>
#include <linux/kobject.h>

#include "WaitFreeQueue.h"

WaitFreeQueue* allocateAndInitializeWaitFreeQueue(unsigned long maxMessageSize, unsigned long maxStorageSize, struct kobject *kObject) {

    WaitFreeQueue* output = kmalloc(sizeof(WaitFreeQueue), GFP_KERNEL);
    if (output == NULL) {

        printk("Kmalloc failed\n");
        return NULL;

    } else {

        output->maxMessageSize = maxStorageSize;
        output->maxStorageSize = maxStorageSize;
        output->kObject = kObject;

        return output;
    }
}