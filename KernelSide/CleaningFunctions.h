#include "./DataStructure/RCUSynchronizer.h"
#include <linux/kobject.h>
#include <linux/sysfs.h>

void fullRemoveMessage(void *input);

void fullRemoveDeviceFileInstanceRBTreeSynchronizer(RCUSynchronizer *input);