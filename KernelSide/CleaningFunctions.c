#include <linux/slab.h>
#include <linux/kobject.h>
#include <linux/sysfs.h>

#include "CleaningFunctions.h"

#include "Common/BasicOperations.h"
#include "Common/BasicDefines.h"

#include "DataStructure/RBTree.h"
#include "DataStructure/RCUSynchronizer.h"
#include "DataStructure/SemiLockFreeQueue.h"
#include "DataStructure/Message.h"
#include "DataStructure/DeviceFileInstance.h"
#include "DataStructure/Session.h"

void fullRemoveMessage(void *input) {
    freeMessage((Message *) input);
}

void fullRemoveSemiLockFreeQueue(void *input) {
    freeSemiLockFreeQueue((SemiLockFreeQueue *) input, &fullRemoveMessage);
}

void fullRemoveSession(void *input) {
    freeSession((Session *) input);
}

void fullRemoveDeviceFileInstance(void *input) {

    DeviceFileInstance *deviceFileInstance;

    deviceFileInstance = (DeviceFileInstance *) input;

    freeKObject(deviceFileInstance->KObject);
    freeRBTreeContentIncluded(deviceFileInstance->activeSessions, &fullRemoveSession);
    freeRCUSynchronizer(deviceFileInstance->semiLockFreeQueueRCUSynchronizer, &fullRemoveSemiLockFreeQueue);
}

void fullRemoveDeviceFileInstanceRBTreeSynchronizer(RCUSynchronizer *input) {

    freeRBTreeContentIncluded(input->RCUProtectedDataStructure, &fullRemoveDeviceFileInstance);
    kfree(input);
}


