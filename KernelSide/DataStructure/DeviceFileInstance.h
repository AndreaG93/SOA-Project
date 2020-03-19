#pragma once

#include <linux/kobject.h>
#include <linux/spinlock_types.h>

#include "KObjectManagement.h"
#include "RCUSynchronizer.h"
#include "RBTree.h"
#include "Session.h"

typedef struct {

    RCUSynchronizer *semiLockFreeQueueRCUSynchronizer;
    RBTree *activeSessions;
    spinlock_t activeSessionsSpinlock;
    struct kobject *KObject;

} DeviceFileInstance;

DeviceFileInstance *allocateDeviceFileInstance(unsigned int minorDeviceNumber, struct kobject* parentKObject,
                                               ssize_t (*show)(struct kobject *, struct kobj_attribute *, char *),
                                               ssize_t (*store)(struct kobject *, struct kobj_attribute *, const char *,
                                                                size_t));

DeviceFileInstance *getDeviceFileInstanceFromSynchronizer(RCUSynchronizer *input, unsigned int minorDeviceNumber);

void freeDeviceFileInstance(DeviceFileInstance *input);

void registerSessionIntoDeviceFileInstance(DeviceFileInstance *input, Session *session);

void unregisterSessionFromDeviceFileInstance(DeviceFileInstance *input, Session *session);