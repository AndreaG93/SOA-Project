#include "./DataStructure/RCUSynchronizer.h"

RCUSynchronizer *getQueueRCUSynchronizer(RCUSynchronizer *RBTreeSynchronizer, unsigned long queueID);

RCUSynchronizer *
allocateNewQueueRCUSynchronizer(unsigned long queueID,
                                ssize_t (*show)(struct kobject *, struct attribute *, char *),
                                ssize_t (*store)(struct kobject *, struct attribute *, const char *size_t)) {

void fullyRemoveMessage(void *input);

void fullyRemoveRBTreeSynchronizer(RCUSynchronizer* input);