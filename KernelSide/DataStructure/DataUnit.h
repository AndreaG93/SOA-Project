#pragma once

typedef struct {
    void *data;
    unsigned long size;
} DataUnit;

DataUnit *allocateDataUnit(void *data, unsigned long size);

void freeDataUnit(DataUnit *input);