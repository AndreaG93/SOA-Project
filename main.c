#include <stdio.h>
#include "UserSide/FileOperationsTests.h"

int main() {

    fprintf(stderr, "Start 'enqueueDequeueTest'...\n");
    enqueueDequeueTest();

    fprintf(stderr, "Start 'delayedEnqueueDequeueTest'...\n");
    delayedEnqueueDequeueTest();

    return 0;
}