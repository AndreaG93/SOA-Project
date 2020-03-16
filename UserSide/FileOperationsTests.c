#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/ioctl.h>

void readTest() {

    char *inputMessageBuffer = calloc(30, sizeof(char));
    if (inputMessageBuffer == NULL)
        exit(EXIT_FAILURE);

    char *outputMessageBuffer = calloc(30, sizeof(char));
    if (outputMessageBuffer == NULL)
        exit(EXIT_FAILURE);

    errno = 0;
    int fileDescriptor = open("/dev/TMS1", O_RDWR);
    if (fileDescriptor == -1) {

        fprintf(stderr, "[ERROR] %s", strerror(errno));
        exit(EXIT_FAILURE);
    }

    errno = 0;
    if (ioctl(fileDescriptor, 7, 1) != 0){

        fprintf(stderr, "[ERROR] %s", strerror(errno));
        exit(EXIT_FAILURE);
    }

    strcpy(inputMessageBuffer, "Riko");
    if (write(fileDescriptor, inputMessageBuffer, 30) == -1)
        exit(EXIT_FAILURE);

    strcpy(inputMessageBuffer, "Andrea");
    if (write(fileDescriptor, inputMessageBuffer, 30) == -1)
        exit(EXIT_FAILURE);

    if (read(fileDescriptor, outputMessageBuffer, 30) == -1)
        exit(EXIT_FAILURE);

    fprintf(stderr, "Message read: %s\n", outputMessageBuffer);

    if (read(fileDescriptor, outputMessageBuffer, 30) == -1)
        exit(EXIT_FAILURE);

    fprintf(stderr, "Message read: %s\n", outputMessageBuffer);

    free(inputMessageBuffer);
    free(outputMessageBuffer);

    close(fileDescriptor);
}