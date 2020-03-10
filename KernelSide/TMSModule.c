#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/err.h>
#include <linux/device.h>

#include "ModuleInformations.h"
#include "TMSModule.h"
#include "TMSDeviceDriver.h"

static struct class *TMSClass = NULL;

/*
void bho(void) {

    snprintf(nodeName, sizeof(nodeName), MODULE_NODE_NAME"%d", i);

    mycdev_devices[i].device = device_create(asp_mycdev_class, NULL,\
			MKDEV(mycdev_major, mycdev_minor + i), NULL, nodeName);
    if(IS_ERR_OR_NULL(mycdev_devices[i].device))
    {

        printk(KERN_WARNING "%s: Failed to Create Device Node %s\n", MODULE_NAME, nodeName);
        nodeSetupFailed = true;
        break;
    }
}
*/

void allocationDeviceDriverClass(void) {

    TMSClass = class_create(THIS_MODULE, MODULE_CLASS_NAME);
    if (IS_ERR_OR_NULL(TMSClass))
        printk(KERN_WARNING "'%s': creation failed!\n", MODULE_CLASS_NAME);

}

void freeDeviceDriverClass(void) {

    if(!IS_ERR_OR_NULL(TMSClass)){
        class_destroy(TMSClass);
        TMSClass = NULL;
        printk(KERN_NOTICE "'%s': successfully freed up!\n", MODULE_CLASS_NAME);
    }
}



int init_module() {

    registerTMSDeviceDriver();

    allocationDeviceDriverClass();

    printk(KERN_NOTICE "'%s': module successfully installed!\n", MODULE_NAME);
    return 0;
}


void cleanup_module() {
    freeDeviceDriverClass();
    unregisterTMSDeviceDriver();
    printk(KERN_NOTICE "'%s': module successfully removed!\n", MODULE_NAME);
}