#pragma once

#include <linux/workqueue.h>

typedef struct {
    void *content;
    unsigned long size;
} __randomize_layout
Message;

Message *allocateMessage(const char *userBuffer, unsigned long userBufferSize);

int copyMessageToUserBuffer(Message *input, void *userBuffer, unsigned long userBufferSize);

void freeMessage(Message *input);

unsigned long getMessageSize(void *input);