#include <linux/workqueue.h>

#include "DataStructure/RCUSynchronizer.h"

#include "Common/BasicDefines.h"

DriverError enqueueMessage(RCUSynchronizer* queueSynchronizer, const char *userBuffer, size_t userBufferSize);

DriverError dequeueMessage(RCUSynchronizer* queueSynchronizer, void *userBuffer, size_t userBufferSize);

void performDelayedMessageEnqueue(struct work_struct *input);