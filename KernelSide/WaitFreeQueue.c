#include "WaitFreeQueue.h"




WaitFreeQueue* allocateAndInitializeWaitFreeQueue(void) {

    WaitFreeQueue* output = kmalloc(sizeof(WaitFreeQueue), GFP_KERNEL);
    if (output == NULL) {

        printk("Kmalloc failed\n");
        return NULL;

    } else {

        output->maxMessageSize = 5;
        output->maxStorageSize = 5;


        return output;
    }
}