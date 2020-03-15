#pragma once

#include <linux/types.h>

typedef struct {
    void *content;
    size_t size;
} Message;

Message *createMessageFromUserBuffer(const char *userBuffer, size_t userBufferSize);

int copyMessageToUserBuffer(Message *input, void *userBuffer, size_t userBufferSize);

void freeMessage(Message *input);