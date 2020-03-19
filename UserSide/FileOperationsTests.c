#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/ioctl.h>

void enqueueDequeueTest() {

    int fileDescriptor;
    char *buffer1;
    char *buffer2;

    buffer1 = calloc(6, sizeof(char));
    if (buffer1 == NULL)
        exit(EXIT_FAILURE);

    buffer2 = calloc(4, sizeof(char));
    if (buffer2 == NULL)
        exit(EXIT_FAILURE);

    errno = 0;
    fileDescriptor = open("/dev/TMS1", O_RDWR);
    if (fileDescriptor == -1) {

        fprintf(stderr, "[ERROR] %s", strerror(errno));
        exit(EXIT_FAILURE);
    }

    if (write(fileDescriptor, &"Andrea", 6) == -1)
        exit(EXIT_FAILURE);

    if (write(fileDescriptor, &"Riko", 4) == -1)
        exit(EXIT_FAILURE);

    if (read(fileDescriptor, buffer1, 6) == -1)
        exit(EXIT_FAILURE);

    if (read(fileDescriptor, buffer2, 4) == -1)
        exit(EXIT_FAILURE);

    fprintf(stderr, "Message 1 read: %s\n", buffer1);
    fprintf(stderr, "Message 2 read: %s\n", buffer2);

    close(fileDescriptor);
}

void delayedEnqueueDequeueTest() {

    int fileDescriptor;
    char *buffer1;


    buffer1 = calloc(6, sizeof(char));
    if (buffer1 == NULL)
        exit(EXIT_FAILURE);

    errno = 0;
    fileDescriptor = open("/dev/TMS2", O_RDWR);
    if (fileDescriptor == -1) {

        fprintf(stderr, "[ERROR] %s", strerror(errno));
        exit(EXIT_FAILURE);
    }

    ioctl(fileDescriptor, 5, 2000);

    if (write(fileDescriptor, &"Andrea", 6) == -1)
        exit(EXIT_FAILURE);

    if (read(fileDescriptor, buffer1, 6) == -1)
        exit(EXIT_FAILURE);

    fprintf(stderr, "Message 1 read: %s\n", buffer1);

    close(fileDescriptor);
}