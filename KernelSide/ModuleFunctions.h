#include "./DataStructure/RCUSynchronizer.h"
#include <linux/kobject.h>
#include <linux/sysfs.h>

void fullyRemoveMessage(void *input);

void fullyRemoveRBTreeSynchronizer(RCUSynchronizer *input);