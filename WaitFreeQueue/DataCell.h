#pragma once

#include "Requests.h"

typedef struct {

    void *data;
    EnqueueRequest *enqueueRequest;
    DequeueRequest *dequeueRequest;

} DataCell;

