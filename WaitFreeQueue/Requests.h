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

inline unsigned long getPendingBitFlagFromRequestState(RequestState input);

inline unsigned long getIdentifierFromRequestState(RequestState input);