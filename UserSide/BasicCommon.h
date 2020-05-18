#pragma once

int getDeviceFileDescriptor(const char *path);

void *getCharBuffer(size_t size);

void readAndClose(const char *deviceFileName, int usingBufferSize);

void writeAndClose(const char *deviceFileName, const char *data, size_t dataSize);

void readWithNoClose(int fileDescriptor);

void writeWithNoClose(int fileDescriptor, const char *data, size_t dataSize);