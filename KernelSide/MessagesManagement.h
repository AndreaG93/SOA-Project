#include <linux/workqueue.h>

#include "DataStructure/RCUSynchronizer.h"
#include "DataStructure/Session.h"

#include "Common/BasicDefines.h"

DriverError enqueueMessage(RCUSynchronizer* queueSynchronizer, const char *userBuffer, size_t userBufferSize);

DriverError dequeueMessage(RCUSynchronizer* queueSynchronizer, void *userBuffer, size_t userBufferSize);

DriverError enqueueDelayedMessage(Session* session, const char *userBuffer, size_t userBufferSize);