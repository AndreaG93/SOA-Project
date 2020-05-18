#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <wait.h>
#include "BasicCommon.h"

int getDeviceFileDescriptor(const char *path) {

    int output = open(path, O_RDWR);
    if (output == -1) {

        fprintf(stderr, "[ERROR] Error code %d", output);
        exit(EXIT_FAILURE);
    } else
        return output;
}

void *getCharBuffer(size_t size) {

    char *output = calloc(size, sizeof(char));

    if (output == NULL)
        exit(EXIT_FAILURE);
    else
        return output;
}

void writeAndClose(const char *deviceFileName, const char *data, size_t dataSize) {

    int fileDescriptor = getDeviceFileDescriptor(deviceFileName);

    if (write(fileDescriptor, data, dataSize) == -1)
        exit(EXIT_FAILURE);

    close(fileDescriptor);
}

void readAndClose(const char *deviceFileName, int usingBufferSize) {

    int fileDescriptor = getDeviceFileDescriptor(deviceFileName);

    char *buffer = getCharBuffer(usingBufferSize);

    if (read(fileDescriptor, buffer, usingBufferSize) == -1)
        exit(EXIT_FAILURE);
    else
        fprintf(stderr, "I have read: %s\n", buffer);

    free(buffer);
    close(fileDescriptor);
}

void readWithNoClose(int fileDescriptor) {

    char *buffer = getCharBuffer(150);

    if (read(fileDescriptor, buffer, 150) == -1)
        exit(EXIT_FAILURE);
    else
        fprintf(stderr, "I have read: %s\n", buffer);

    free(buffer);
}

void writeWithNoClose(int fileDescriptor, const char *data, size_t dataSize) {

    if (write(fileDescriptor, data, dataSize) == -1)
        exit(EXIT_FAILURE);
}