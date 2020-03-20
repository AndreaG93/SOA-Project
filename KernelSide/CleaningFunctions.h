#include <linux/kobject.h>
#include <linux/sysfs.h>

#include "DataStructure/Session.h"
#include "DataStructure/RCUSynchronizer.h"

void fullRemoveMessage(void *input);

void fullRemoveDeviceFileInstanceRBTreeSynchronizer(RCUSynchronizer *input);

void cleanQueue(Session* session);