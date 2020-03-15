#include "./DataStructure/RCUSynchronizer.h"

RCUSynchronizer *getQueueRCUSynchronizer(RCUSynchronizer *RBTreeSynchronizer, unsigned long queueID);

RCUSynchronizer *allocateNewQueueRCUSynchronizer(void);

void fullyRemoveMessage(void *input);

void fullyRemoveRBTreeSynchronizer(RCUSynchronizer* input);