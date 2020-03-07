//
// Created by andrea on 06/03/20.
//

#include <stdbool.h>
#include <stddef.h>
#include <sys/user.h>
#include <malloc.h>
#include "WaitFreeQueue.h"
#include "../Hardware/SynchronizationPrimitives.h"
#include "../Hardware/MemoryAlignment.h"


#define FAST_PATH_MAX_STEPS 10
#define N WAIT_FREE_QUEUE_NODE_SIZE
#define SUCCESS 0

WaitFreeQueueNode *_allocateAndInitializeWaitFreeQueueNode(unsigned long identifier) {

    WaitFreeQueueNode *output = NULL;//allocateAlignedMemoryArea(PAGE_SIZE, sizeof(WaitFreeQueueNode));

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

static DataCell *_findDataCell(WaitFreeQueueNode **validNode, unsigned long targetCellIdentifier) {

    unsigned long currentIdentifier;

    WaitFreeQueueNode *currentNode = *validNode;
    WaitFreeQueueNode *nextNode = NULL;

    for (currentIdentifier = currentNode->identifier;
         currentIdentifier < targetCellIdentifier / N; currentIdentifier++) {

        nextNode = (WaitFreeQueueNode *) currentNode->next;

        if (nextNode == NULL) {

            WaitFreeQueueNode *newNode = _allocateAndInitializeWaitFreeQueueNode(currentIdentifier + 1);
            if (!CAS(&currentNode->next, NULL, newNode))
                free(newNode);

            nextNode = (WaitFreeQueueNode *) currentNode->next;
        }

        currentNode = nextNode;
    }

    *validNode = currentNode;
    return &currentNode->cells[currentIdentifier % N];
}

WaitFreeQueue *allocateAndInitializeWaitFreeQueue() {

    WaitFreeQueue *output = NULL;//allocateAlignedMemoryArea(PAGE_SIZE, sizeof(WaitFreeQueue));
    output->headIndex = 0;
    output->tailIndex = 0;
    output->nodeList = _allocateAndInitializeWaitFreeQueueNode(0);

    return output;
}

bool _isEnqueueFastSuccessful(WaitFreeQueue *waitFreeQueue, ThreadLocalState *threadLocalState, void *data) {

    unsigned long targetDataCellIndex = FAA(&waitFreeQueue->tailIndex, 1);
    DataCell *targetDataCell = _findDataCell(&threadLocalState->tailNode, targetDataCellIndex);

    if (CAS(&targetDataCell->data, NULL, data))
        return true;
    else {
        //*cid := i;
        return false;
    }
}

void _enqueueSlow (WaitFreeQueue *waitFreeQueue, ThreadLocalState *threadLocalState, void *data , int cell_id ) {

    /*

// publish enqueue request
    r := &h->enq.req;
    r - > val := v;
    r - > state := (1, cell_id);
// use a local tail pointer to traverse because
// line 87 may need to find an earlier cell .
    tmp_tail := h->tail;
    do {
// obtain new cell index and locate candidate cell
        i := FAA (&q->T, 1);
        c := find_cell(&tmp_tail, i);
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

    for (unsigned int currentStep = 0; currentStep < FAST_PATH_MAX_STEPS; currentStep++)
        if (_isEnqueueFastSuccessful(waitFreeQueue, threadLocalState, data) == true)
            return;

    _enqueueSlow(waitFreeQueue, threadLocalState, data, 0);
}

ThreadLocalState* allocateAndInitializeThreadLocalState(WaitFreeQueue* waitFreeQueue) {

    ThreadLocalState* output = NULL;//allocateAlignedMemoryArea(PAGE_SIZE, sizeof(ThreadLocalState));

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