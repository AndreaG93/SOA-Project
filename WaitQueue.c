//
// Created by andrea on 06/03/20.
//

#include <stdbool.h>
#include "WaitQueue.h"

#define FAST_PATH_MAX_STEPS 10
#define SUCCESS 0


// https://www.felixcloutier.com/x86/cmpxchg
inline unsigned int CAS(volatile uint32_t *mem, uint32_t with, uint32_t cmp) {
    uint32_t prev = cmp;
    // This version by Mans Rullgard of Pathscale
    __asm__ __volatile__ ( "lock\n\t"
                           "cmpxchg %2,%0"
    : "+m"(*mem), "+a"(prev)
    : "r"(with)
    : "cc");

    return prev;
}


bool _isEnqueueFastSucceded(WaitFreeQueue *waitFreeQueue, ThreadLocalState* threadLocalState, void *data) {

    unsigned long candidateCellIndex := FAA (&q ->T , 1);
    c := find_cell(& h - > tail , i );
    if ( CAS (& c. val , ?, v ))
        return true ;
    else {
        *cid := i;
        return false;
    }
}

void _enqueueSlow(WaitFreeQueue *waitFreeQueue, ThreadLocalState* threadLocalState, void *data) {
    do t := FAA(&T, 1);
    while (!CAS(&Q[t], ?, x));
}


void enqueue(WaitFreeQueue *waitFreeQueue, ThreadLocalState *threadLocalState, void *data) {

    for (unsigned int currentStep = 0; currentStep < FAST_PATH_MAX_STEPS; currentStep++)
        if (_enqueueFast(waitFreeQueue, threadLocalState, data) == SUCCESS)
            return;

    _enqueueSlow(waitFreeQueue, threadLocalState, data);
}

