//
// Created by andrea on 06/03/20.
//

#include <stdbool.h>
#include <stddef.h>
#include <sys/user.h>
#include <malloc.h>
#include "WaitFreeQueue.h"
#include "../Hardware/SynchronizationPrimitives.h"


#define FAST_PATH_MAX_STEPS 10
#define N WAIT_FREE_QUEUE_NODE_SIZE
#define SUCCESS 0

WaitFreeQueue* allocateWaitFreeQueue() {




    return NULL;
}

WaitFreeQueueNode* allocateWaitFreeQueueNode(unsigned long identifier) {

    WaitFreeQueueNode* output = NULL; //align_malloc(PAGE_SIZE, sizeof(WaitFreeQueueNode));

    output->identifier = identifier;
    output->next = NULL;

    for (int index = 0; index < WAIT_FREE_QUEUE_NODE_SIZE; index++) {
        output->cells[index];
    }

    return output;
}



static DataCell* findTargetCell(WaitFreeQueueNode* node, unsigned long targetCellIdentifier) {

    unsigned long currentIdentifier;

    WaitFreeQueueNode* currentNode = node;
    WaitFreeQueueNode* nextNode = NULL;

    for (currentIdentifier = currentNode->identifier; currentIdentifier < targetCellIdentifier / N; currentIdentifier++) {

        nextNode = currentNode->next;

        if (nextNode == NULL) {

            WaitFreeQueueNode* tmp = allocateWaitFreeQueueNode(currentIdentifier + 1);
            if (!CAS(&node->next , NULL , tmp )) {
                free(tmp);
            }

            nextNode = currentNode->next;
        }

        currentNode = nextNode;
    }

    *node = *currentNode;
    return &currentNode->cells[currentIdentifier % N];
}



/*
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
 */

/*
bool _isEnqueueFastSuccessful(WaitFreeQueue *waitFreeQueue, ThreadLocalState* threadLocalState, void *data) {

    unsigned long candidateCellIndex = FAA(&waitFreeQueue->tailIndex , 1);
    Cell *targetCell = findTargetCell(&threadLocalState->tailNode, candidateCellIndex);

    if (CAS(& targetCell->data, ?, v ))
        return true ;
    else {
        *cid := i;
        return false;
    }
}

void _enqueueSlow(WaitFreeQueue *waitFreeQueue, ThreadLocalState* threadLocalState, void *data) {
    do {
        t := FAA(&T, 1);
    }
    while (!CAS(&Q[t], ?, x));
}


void enqueue(WaitFreeQueue *waitFreeQueue, ThreadLocalState *threadLocalState, void *data) {

    for (unsigned int currentStep = 0; currentStep < FAST_PATH_MAX_STEPS; currentStep++)
        if (_enqueueFast(waitFreeQueue, threadLocalState, data) == SUCCESS)
            return;

    _enqueueSlow(waitFreeQueue, threadLocalState, data);
}

*/