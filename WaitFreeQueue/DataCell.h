#pragma once

#include "../Hardware/MemoryAlignment.h"
#include "Requests.h"

typedef struct {

    void *data;
    EnqueueRequest *enqueueRequest;
    DequeueRequest *dequeueRequest;

} DataCell;

