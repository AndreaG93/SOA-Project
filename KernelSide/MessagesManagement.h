#include "DataStructure/RCUSynchronizer.h"

unsigned int enqueueMessage(RCUSynchronizer* queueSynchronizer, const char *userBuffer, size_t userBufferSize);

unsigned int dequeueMessage(RCUSynchronizer* queueSynchronizer, const char *userBuffer, size_t userBufferSize);