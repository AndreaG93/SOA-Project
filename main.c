//
// Created by andrea on 06/03/20.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "WaitFreeQueue/LockFreeQueue.h"

// alternative: union with  unsigned __int128


typedef struct {
    void *lo;
    void *hi;
} DCASOperand __attribute__ (( __aligned__( 16 )));


int _CAS2(DCASOperand *ptr, void *cmp1, void *cmp2, void *val1, void *val2) {
    char success;

    __asm__ __volatile__(
    "lock cmpxchg16b %1\n"
    "setz %0"
    : "=q" (success), "+m" (*ptr), "+a" (cmp1), "+d" (cmp2)
    : "b" (val1), "c" (val2)
    : "cc" );

    return success;
}


int main() {


    void *input = "Luca";
    void *input2 = "DD";

    DCASOperand operand;
    operand.lo = input;
    operand.hi = input2;

    void *lo = "Luca";
    void *hi = "DD";


    _CAS2(&operand, lo, hi, "Andrea", "Graziani");

    fprintf(stderr, "%s, %s\n", operand.lo, operand.hi);
    fprintf(stderr, "%s, %s\n", input, input2);
     /*

    const char* ff = "Andrea";
    char *string = malloc(sizeof(char) * 6);
    memcpy(string, ff, 6);

    fprintf(stderr, "%s", string);

    LockFreeQueue* lockFreeQueue = allocateAndInitializeLockFreeQueue(10, 100);

    enqueue(lockFreeQueue, string);

    char *output = dequeue(lockFreeQueue);
    fprintf(stderr, "%s", output);
*/
    return 0;
}