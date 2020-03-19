#pragma once

#include <linux/kobject.h>
#include <linux/spinlock_types.h>

#include "KObjectManagementFunctions.h"
#include "RCUSynchronizer.h"
#include "RBTree.h"

typedef struct {

    RCUSynchronizer* semiLockFreeQueueRCUSynchronizer;
    RBTree *activeSessions;
    spinlock_t activeSessionsSpinlock;
    struct kobject *KObject;

} DeviceFileInstance;

DeviceFileInstance *allocateDeviceFileInstance(unsigned int minorDeviceNumber, KObjectManagementFunctions *functions);

DeviceFileInstance *getDeviceFileInstanceFromSynchronizer(RCUSynchronizer *input, unsigned int minorDeviceNumber);

void freeDeviceFileInstance(DeviceFileInstance* input);

void registerSessionIntoDeviceFileInstance(DeviceFileInstance* input, Session *session);