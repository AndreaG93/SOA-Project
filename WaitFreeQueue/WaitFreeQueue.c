//
// Created by andrea on 06/03/20.
//

#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include "WaitFreeQueue.h"
#include "../KernelSide/BasicOperations/SynchronizationPrimitives.h"
#include "../KernelSide/BasicOperations/BasicDefines.h"

#define FAST_PATH_MAX_ATTEMPTS 10
#define N WAIT_FREE_QUEUE_NODE_SIZE

WaitFreeQueueNode *allocateAndInitializeWaitFreeQueueNode(unsigned long identifier) {

    WaitFreeQueueNode *output = malloc(sizeof(WaitFreeQueueNode));
    if (output == NULL)
        exit(EXIT_FAILURE);

    output->identifier = identifier;
    output->next = NULL;

    for (int index = 0; index < WAIT_FREE_QUEUE_NODE_SIZE; index++) {

        DataCell currentDatCell = output->cells[index];

        currentDatCell.enqueueRequest = NULL;
        currentDatCell.dequeueRequest = NULL;
        currentDatCell.data = NULL;
    }

    return output;
}

DataCell *findDataCell(WaitFreeQueueNode **validNode, unsigned long targetCellIdentifier) {

    unsigned long currentIdentifier;

    WaitFreeQueueNode *currentNode = *validNode;
    WaitFreeQueueNode *nextNode = NULL;

    for (currentIdentifier = currentNode->identifier; currentIdentifier < targetCellIdentifier / N; currentIdentifier++) {

        nextNode = (WaitFreeQueueNode *) currentNode->next;

        if (nextNode == NULL) {

            WaitFreeQueueNode *newNode = allocateAndInitializeWaitFreeQueueNode(currentIdentifier + 1);
            if (COMPARE_AND_SWAP(&currentNode->next, NULL, newNode) == FAILURE)
                free(newNode);

            nextNode = (WaitFreeQueueNode *) currentNode->next;
        }

        currentNode = nextNode;
    }

    *validNode = currentNode;
    return &currentNode->cells[currentIdentifier % N];
}

WaitFreeQueue *allocateAndInitializeWaitFreeQueue() {

    WaitFreeQueue *output = malloc(sizeof(WaitFreeQueue));
    if (output == NULL)
        exit(EXIT_FAILURE);

    output->headIndex = 0;
    output->tailIndex = 0;
    output->maxStorageSize = 10;
    output->maxMessageSize = 15;

    output->nodeList = allocateAndInitializeWaitFreeQueueNode(0);

    return output;
}

int fastEnqueue(WaitFreeQueue *waitFreeQueue, ThreadLocalState *threadLocalState, void *data) {

    unsigned long targetDataCellIndex = FETCH_AND_ADD(&waitFreeQueue->tailIndex, 1);
    DataCell *targetDataCell = findDataCell(&threadLocalState->tailNode, targetDataCellIndex);

    if (COMPARE_AND_SWAP(&(targetDataCell->data), NULL, data) == SUCCESS)
        return SUCCESS;
    else {
        //*cid := i;
        return FAILURE;
    }
}

void slowEnqueue(WaitFreeQueue *waitFreeQueue, ThreadLocalState *threadLocalState, void *data , int targetCellId) {

    unsigned long targetDataCellIndex;
    DataCell *targetDataCell;

    EnqueueRequest* threadEnqueueRequest = &threadLocalState->Enqueue.enqueueRequest;

    threadEnqueueRequest->data = data;
    threadEnqueueRequest->state = PENDING_REQUEST;
    threadEnqueueRequest->id = targetCellId;

    WaitFreeQueueNode* tempTail = threadLocalState->tailNode;
    do {

        targetDataCellIndex = FETCH_AND_ADD(&waitFreeQueue->tailIndex, 1);
        targetDataCell = findDataCell(&tempTail, targetDataCellIndex);

        if (COMPARE_AND_SWAP(&targetDataCell->enqueueRequest, NULL, threadEnqueueRequest) && targetDataCell->data == NULL) {

            if (COMPARE_AND_SWAP(&threadEnqueueRequest->id, &targetCellId, -targetDataCellIndex) == SUCCESS)
                targetCellId = -targetDataCellIndex;

            break;
        }

    } while (threadEnqueueRequest->state == PENDING_REQUEST);

    targetCellId = -(threadEnqueueRequest->id);
    targetDataCell = findDataCell(&tempTail, targetDataCellIndex);


    /*

// Dijkstra ’s protocol
        if (CAS (&c->enq, ?e, r) and
        c.val = ?) {
            try_to_claim_req(&r - > state, id, i); // for cell i
// invariant : request claimed ( even if CAS failed )
            break;
        }
    } while (r->state.pending);
// invariant : req claimed for a cell ; find that cell
    id := r->state.id;
    c := find_cell(&h - > tail, id);
    enq_commit(q, c, v, id);
// invariant : req committed before enq_slow returns
     */
}


void enqueue(WaitFreeQueue *waitFreeQueue, ThreadLocalState *threadLocalState, void *data) {

    for (unsigned int currentAttempt = 0; currentAttempt < FAST_PATH_MAX_ATTEMPTS; currentAttempt++)
        if (fastEnqueue(waitFreeQueue, threadLocalState, data) == SUCCESS)
            return;

    slowEnqueue(waitFreeQueue, threadLocalState, data, 0);
}

ThreadLocalState* allocateAndInitializeThreadLocalState(WaitFreeQueue* waitFreeQueue) {

    ThreadLocalState* output = malloc(sizeof(ThreadLocalState));
    if (output == NULL)
        exit(EXIT_FAILURE);

    output->headNode = waitFreeQueue->nodeList;
    output->tailNode = waitFreeQueue->nodeList;

    output->next = NULL;
    output->Dequeue.peer = NULL;
    output->Enqueue.peer = NULL;

    return output;
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