#pragma once

typedef struct {
    void *content;
    size_t size;
} Message;

Message *allocateMessage(void *userBuffer, size_t userBufferSize);

unsigned char copyMessageToUserBuffer(Message *input, void *userBuffer, size_t userBufferSize);

void freeMessage(Message *input);