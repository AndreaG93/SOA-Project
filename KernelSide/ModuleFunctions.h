#include "./DataStructure/RCUSynchronizer.h"
#include <linux/kobject.h>
#include <linux/sysfs.h>

RCUSynchronizer *getQueueRCUSynchronizer(RCUSynchronizer *RBTreeSynchronizer, unsigned long queueID);

RCUSynchronizer *
allocateNewQueueRCUSynchronizer(unsigned long queueID, struct kobject *kObjectParent, ssize_t (*show)(struct kobject *, struct kobj_attribute *, char *),
                                ssize_t (*store)(struct kobject *, struct kobj_attribute *, const char *, size_t));

void fullyRemoveMessage(void *input);

void fullyRemoveRBTreeSynchronizer(RCUSynchronizer *input);