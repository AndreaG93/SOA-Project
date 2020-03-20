#include <stdio.h>
#include "UserSide/FileOperationsTests.h"

int main() {

/*
    fprintf(stderr, "Start 'emptyTest'...\n");
    emptyTest();

    fprintf(stderr, "Start 'enqueueDequeueTest'...\n");
    enqueueDequeueTest();

    fprintf(stderr, "Start 'delayedEnqueueDequeueTest'...\n");
    delayedEnqueueDequeueTest();

    fprintf(stderr, "Start 'failedDelayedEnqueueDequeueTest'...\n");
    failedDelayedEnqueueDequeueTest();

    fprintf(stderr, "Start 'delayedDequeueTest'...\n");
    delayedDequeueTest();
*/
    fprintf(stderr, "Start 'cancelledDelayedDequeueTest'...\n");
    cancelledDelayedDequeueTest();

    return 0;
}