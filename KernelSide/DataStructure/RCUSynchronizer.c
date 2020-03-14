#include "RCUSynchronizer.h"

#include <linux/spinlock_types.h>

RCUSynchronizer *allocateRCUSynchronizer(void *dataStructureToProtect) {

    RCUSynchronizer *output = kmalloc(sizeof(RCUSynchronizer), GFP_KERNEL);
    if (output != NULL) {

        output->epoch = 0;
        output->standing[0] = 0;
        output->standing[1] = 0;
        output->RCUProtectedDataStructure = dataStructureToProtect;
        spin_lock_init(output->writeLock)

        asm volatile("mfence");
    }

    return output;
}

unsigned int readLockRCUGettingEpoch(RCUSynchronizer *input) {

    unsigned output = input->epoch;

    asm volatile("mfence");
    __sync_add_and_fetch(&input->standing[output], 1);

    return output;
}

void readUnlockRCU(RCUSynchronizer *input, unsigned int epoch) {
    __sync_sub_and_fetch(&input->standing[epoch], 1);
}


void writeLockRCU(RCUSynchronizer *input) {
    spin_lock(&input->writeLock)
}

void writeUnlockRCU(RCUSynchronizer *input, void *newDataStructureToProtect) {

    unsigned int gracePeriod = input->epoch;

    input->RCUProtectedDataStructure = newDataStructureToProtect;
    input->epoch = (input->epoch + 1) % 2;

    asm volatile("mfence");

    while (input->standing[gracePeriod] > 0);

    spin_unlock(&input->writeLock)
}