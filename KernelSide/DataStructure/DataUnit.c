#include "DataUnit.h"

#include <linux/slab.h>

DataUnit *allocateDataUnit(void *data, unsigned long size) {

    DataUnit *output = kmalloc(sizeof(NodeContent), GFP_KERNEL);
    if (output != NULL) {

        output->data = data;
        output->size = size;
    }

    return output;
}

void freeDataUnit(DataUnit *input) {

    kfree(input->data);
    kfree(input);
}