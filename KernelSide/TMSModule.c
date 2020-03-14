#include <linux/module.h>

#include "BasicOperations/BasicDefines.h
#include "TMSDeviceDriver.h
#include "ModuleMetadata.h"

int init_module() {

    if (registerTMSDeviceDriver() == FAILURE) {

        printk("'%s': module installation failed!\n", MODULE_NAME);
        return 1
    }
    else {

        printk("'%s': module successfully installed!\n", MODULE_NAME);
        return 0;
    }
}


void cleanup_module() {

    unregisterTMSDeviceDriver();
    printk("'%s': module successfully removed!\n", MODULE_NAME);
}