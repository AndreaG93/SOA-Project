//
// Created by andrea on 08/03/20.
//

#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>

#include "FileOperationsTests.h"

void readTest() {

    int fileDescriptor;
    char buffer[6];

    errno = 0;
    fileDescriptor = open("/dev/TMS1", O_RDONLY);

    if (fileDescriptor == 1) {
        fprintf(stderr, "[ERROR] %s", strerror(errno));
        exit(EXIT_FAILURE);
    }

    if (read(fileDescriptor, buffer,6) < 0) {
        fprintf(stderr, "[ERROR] %s", strerror(errno));
        exit(EXIT_FAILURE);
    } else
        fprintf(stdout, "Value Read: %s\n", buffer);

    errno = 0;
    if (close(fileDescriptor) != 0) {
        fprintf(stderr, "[ERROR] %s", strerror(errno));
        exit(EXIT_FAILURE);
    }
}