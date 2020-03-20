#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/ioctl.h>


void emptyTest() {

    int fileDescriptor;

    fileDescriptor = open("/dev/TMS0", O_RDWR);
    if (fileDescriptor != 0) {

        fprintf(stderr, "[ERROR] Error code %d", fileDescriptor);
        exit(EXIT_FAILURE);
    }

    fprintf(stderr, "Error code: %lu!\n", read(fileDescriptor, NULL, 10));

    close(fileDescriptor);
}

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


    fileDescriptor = open("/dev/TMS1", O_RDWR);
    if (fileDescriptor == -1) {

        fprintf(stderr, "[ERROR] Error code %d", fileDescriptor);
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
    int output;
    char *buffer1;


    buffer1 = calloc(6, sizeof(char));
    if (buffer1 == NULL)
        exit(EXIT_FAILURE);

    fileDescriptor = open("/dev/TMS2", O_RDWR);
    if (fileDescriptor == -1) {

        fprintf(stderr, "[ERROR] Error 55 code %d", fileDescriptor);
        exit(EXIT_FAILURE);
    }

    ioctl(fileDescriptor, 5, 1000);

    if (write(fileDescriptor, &"Andrea", 6) == -1)
        exit(EXIT_FAILURE);

    while (1) {
        output = read(fileDescriptor, buffer1, 6);
        if (output == 0)
            break;
    }

    fprintf(stderr, "Message 1 read: %s\n", buffer1);

    close(fileDescriptor);
}

void failedDelayedEnqueueDequeueTest() {

    int fileDescriptor;

    fileDescriptor = open("/dev/TMS2", O_RDWR);
    if (fileDescriptor == -1) {

        fprintf(stderr, "[ERROR] Error code %d", fileDescriptor);
        exit(EXIT_FAILURE);
    }

    ioctl(fileDescriptor, 5, 1000);

    if (write(fileDescriptor, &"Andrea", 6) == -1)
        exit(EXIT_FAILURE);

    close(fileDescriptor);
}

void delayedDequeueTest() {

    int fileDescriptor;
    char *buffer1;

    buffer1 = calloc(6, sizeof(char));
    if (buffer1 == NULL)
        exit(EXIT_FAILURE);

    fileDescriptor = open("/dev/TMS1", O_RDWR);
    if (fileDescriptor == -1) {

        fprintf(stderr, "[ERROR] Error code %d", fileDescriptor);
        exit(EXIT_FAILURE);
    }

    if (write(fileDescriptor, &"Chika", 6) == -1)
        exit(EXIT_FAILURE);

    ioctl(fileDescriptor, 6, 2000);

    if (read(fileDescriptor, buffer1, 6) == -1)
        exit(EXIT_FAILURE);

    fprintf(stderr, "Message 1 read: %s\n", buffer1);

    close(fileDescriptor);
}

void cancelledDelayedDequeueTest() {

    int fileDescriptor;
    char *buffer;

    int output = fork();

    if (output == 0) {

        buffer = calloc(6, sizeof(char));
        if (buffer == NULL)
            exit(EXIT_FAILURE);

        fileDescriptor = open("/dev/TMS1", O_RDWR);
        if (fileDescriptor == -1) {

            fprintf(stderr, "[ERROR] Error code %d", fileDescriptor);
            exit(EXIT_FAILURE);
        }

        if (write(fileDescriptor, &"Chika", 6) == -1)
            exit(EXIT_FAILURE);

        ioctl(fileDescriptor, 6, 10000);

        if (read(fileDescriptor, buffer, 6) == -1)
            exit(EXIT_FAILURE);

        fprintf(stderr, "Message 1 read: %s\n", buffer);

    } else {


        fileDescriptor = open("/dev/TMS1", O_RDWR);
        if (fileDescriptor == -1) {

            fprintf(stderr, "[ERROR] Error code %d", fileDescriptor);
            exit(EXIT_FAILURE);
        }

        sleep(2);
    }

    close(fileDescriptor);
}