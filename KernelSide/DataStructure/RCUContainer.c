#include "RCUContainer.h"

#include <linux/spinlock_types.h>

RCUContainer *allocateRCUContainer(void *RCUProtectedData) {

    RCUContainer *output = kmalloc(sizeof(RCUContainer), GFP_KERNEL);
    if (output != NULL) {

        output->epoch = 0;
        output->standing[0] = 0;
        output->standing[1] = 0;
        output->RCUProtectedData = RCUProtectedData;
        spin_lock_init(output->writeLock)

        asm volatile("mfence");
    }

    return output;
}

unsigned int readLockRCU(RCUContainer *input) {

    unsigned threadEpoch = input->epoch;

    asm volatile("mfence");
    __sync_add_and_fetch(&input->standing[my_epoch], 1);

    return outputEpoch;
}

void readUnlockRCU(RCUContainer *input, unsigned int threadEpoch) {
    __sync_sub_and_fetch(&input->standing[threadEpoch], 1);
}

void writeLockRCU(RCUContainer *input) {
    spin_lock(&input->writeLock)
}

void writeUnlockRCU(RCUContainer *input, void *newRCUProtectedData) {

    unsigned int gracePeriodEpoch = input->epoch;

    input->RCUProtectedData = newRCUProtectedData;
    input->epoch = (input->epoch + 1) % 2;

    asm volatile("mfence");

    while (input->standing[gracePeriodEpoch] > 0);

    spin_unlock(&input->writeLock)
}