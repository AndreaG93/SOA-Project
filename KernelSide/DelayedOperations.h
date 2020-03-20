#pragma once

#include <linux/workqueue.h>

#include "DataStructure/Message.h"
#include "DataStructure/Session.h"

typedef struct {

    struct delayed_work work;

    unsigned long index;
    Session *session;
    Message* message;

} DelayedEnqueueOperation;

DelayedEnqueueOperation* allocateDelayedEnqueueOperation(Session* session, const char *userBuffer, unsigned long userBufferSize);

void registerAndEnableDelayedEnqueueOperation(DelayedEnqueueOperation* operation, void (* work)(struct work_struct*));

void unregisterDelayedEnqueueOperation(DelayedEnqueueOperation* operation);

void revokeAllDelayedEnqueueOperations(Session *input);

void revokeAllDelayedEnqueueOperationsVoid(void *input);

void revokeAllDelayedDequeueOperationsVoid(void *input);