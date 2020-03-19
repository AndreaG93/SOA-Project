#include "DataStructure/RCUSynchronizer.h"

#include <linux/workqueue.h>

typedef struct {

    struct delayed_work work;
    RCUSynchronizer* queueSynchronizer;
    const char *userBuffer;
    size_t userBufferSize;

} DelayedEnqueueMessageOperation;


DriverError enqueueMessage(RCUSynchronizer* queueSynchronizer, const char *userBuffer, size_t userBufferSize);

DriverError dequeueMessage(RCUSynchronizer* queueSynchronizer, void *userBuffer, size_t userBufferSize);

void enqueueMessageDelayed(struct work_struct *input);