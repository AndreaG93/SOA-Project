#pragma once

#include "../Hardware/MemoryAlignment.h"

typedef unsigned long RequestState;

typedef struct {
    void *data;
    RequestState state;
} EnqueueRequest;

typedef struct {
    unsigned long identifier;
    RequestState state;
} DequeueRequest;

EnqueueRequest* allocateAndInitializeEnqueueRequest();

DequeueRequest* allocateAndInitializeDequeueRequest();

inline unsigned long getPendingBitFlagFromRequestState(RequestState input);

inline unsigned long getIdentifierFromRequestState(RequestState input);