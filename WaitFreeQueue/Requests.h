#pragma once

typedef unsigned long RequestState;

typedef struct {
    void *data;
    unsigned int state;
    unsigned long id;
} EnqueueRequest;

typedef struct {
    unsigned long identifier;
    unsigned int state;
    unsigned long id;
} DequeueRequest;

//inline unsigned long getPendingBitFlagFromRequestState(RequestState input);

//inline unsigned long getIdentifierFromRequestState(RequestState input);