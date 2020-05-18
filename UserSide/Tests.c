#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <wait.h>
#include "BasicCommon.h"

void writeAndReadTest() {

    int readerProcess;
    int writerProcess;

    writerProcess = fork();

    if (writerProcess == 0) {

        writeAndClose("/dev/TMS1", "andrea", 6);
        writeAndClose("/dev/TMS1", "sistemi operativi", 17);
        writeAndClose("/dev/TMS1", "italia", 6);
        return;

    } else
        wait(NULL);

    readerProcess = fork();
    if (readerProcess == 0) {

        readAndClose("/dev/TMS1", 6);
        readAndClose("/dev/TMS1", 10);
        readAndClose("/dev/TMS1", 10);
        return;

    } else
        wait(NULL);
}

void concurrentWriteAndNonBlockingReadTest() {

    int writerProcess;

    writerProcess = fork();

    if (writerProcess == 0) {

        for (int i = 0; i < 10000; i++)
            writeAndClose("/dev/TMS1", "andrea", 6);

        return;
    }

    for (int i = 0; i < 10000; i++)
        readAndClose("/dev/TMS1", 6);

    wait(NULL);
}

void SET_SEND_TIMEOUT_Test() {

    char *buffer = getCharBuffer(150);
    int fileDescriptor = getDeviceFileDescriptor("/dev/TMS2");

    ioctl(fileDescriptor, 5, 1000);

    writeWithNoClose(fileDescriptor, "Andrea", 6);

    fprintf(stderr, "I have written but...");
    while (1) {
        if (read(fileDescriptor, buffer, 6) == 0)
            break;
    }

    fprintf(stderr, "...i have used 'SET_SEND_TIMEOUT' --> Message read: %s\n", buffer);

    free(buffer);
    close(fileDescriptor);
}

void SET_RECV_TIMEOUT_Test() {

    char *buffer = getCharBuffer(150);
    int fileDescriptor = getDeviceFileDescriptor("/dev/TMS2");

    ioctl(fileDescriptor, 6, 1000);

    writeWithNoClose(fileDescriptor, "Andrea", 6);
    fprintf(stderr, "Data fully written but...\n");
    readWithNoClose(fileDescriptor);

    fprintf(stderr, "...i have used 'SET_RECV_TIMEOUT'");

    free(buffer);
    close(fileDescriptor);
}

void REVOKE_DELAYED_MESSAGES_Test() {

    int fileDescriptor = getDeviceFileDescriptor("/dev/TMS2");

    ioctl(fileDescriptor, 5, 1000);

    writeWithNoClose(fileDescriptor, "Andrea", 6);
    writeWithNoClose(fileDescriptor, "Linux", 5);
    writeWithNoClose(fileDescriptor, "Windows", 7);

    fprintf(stderr, "i have called 'write' but...\n");

    ioctl(fileDescriptor, 7);

    fprintf(stderr, "...i have used 'REVOKE_DELAYED_MESSAGES'\n");

    readWithNoClose(fileDescriptor);

    fprintf(stderr, "...so nothing is been written or read!\n");

    close(fileDescriptor);
}

void CLEAN_QUEUE_Test() {

    int fileDescriptor = getDeviceFileDescriptor("/dev/TMS2");

    writeWithNoClose(fileDescriptor, "Andrea", 6);
    writeWithNoClose(fileDescriptor, "Linux", 5);
    writeWithNoClose(fileDescriptor, "Windows", 7);

    fprintf(stderr, "Data fully written but...\n");

    ioctl(fileDescriptor, 8);

    fprintf(stderr, "...i have used 'CLEAN_QUEUE'\n");

    readWithNoClose(fileDescriptor);

    fprintf(stderr, "...so nothing is been read!\n");

    close(fileDescriptor);
}

void concurrentMultipleMinorNumber() {

    int processChild1;
    int processChild2;
    int processChild3;
    int processChild4;

    processChild1 = fork();

    if (processChild1 == 0) {
        for (int i = 0; i < 10000; i++)
            writeAndClose("/dev/TMS0", "andrea-1", 8);
        return;
    }

    processChild2 = fork();

    if (processChild2 == 0) {
        for (int i = 0; i < 10000; i++)
            writeAndClose("/dev/TMS1", "andrea-2", 8);
        return;
    }

    processChild3 = fork();

    if (processChild3 == 0) {
        for (int i = 0; i < 10000; i++)
            readAndClose("/dev/TMS0", 150);
        return;
    }

    processChild4 = fork();

    if (processChild4 == 0) {
        for (int i = 0; i < 10000; i++)
            readAndClose("/dev/TMS1", 150);
        return;
    }

    wait(NULL);
}