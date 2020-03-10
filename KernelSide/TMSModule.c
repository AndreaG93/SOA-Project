#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/err.h>
#include <linux/device.h>

#include "ModuleMetadata.h"
#include "TMSModule.h"
#include "TMSDeviceDriver.h"

int init_module() {

    registerTMSDeviceDriver();

    printk("'%s': module successfully installed!\n", MODULE_NAME);
    return 0;
}


void cleanup_module() {

    unregisterTMSDeviceDriver();
    printk("'%s': module successfully removed!\n", MODULE_NAME);
}