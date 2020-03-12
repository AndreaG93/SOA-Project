//
// Created by andrea on 06/03/20.
//


#include <stddef.h>
#include <stdlib.h>
#include "LockFreeQueue.h"

#define BOT ((void *)0)

#define SUCCESS 0
#define FAILURE 1
#define NOT_ENOUGH_STORAGE_AVAILABLE 2
#define ALLOCATION_FAILED 3
#define EMPTY_QUEUE 4

#define COMPARE_AND_SWAP(dataPointer, expectedValue, desiredValue) __atomic_compare_exchange_n(dataPointer, expectedValue, desiredValue, 0, __ATOMIC_SEQ_CST, __ATOMIC_SEQ_CST)

#define ADD_AND_FETCH(dataPointer, value) __atomic_add_fetch(dataPointer, value, __ATOMIC_SEQ_CST)



typedef struct {
    void *lo;
    void *hi;
} DCASOperand __attribute__ (( __aligned__( 16 )));



int CAS2(volatile DCASOperand sourceOperand, void *lowerCompare, void *upperCompare, void *newLowerValue, void *newUpperValue) {
    char success;

    __asm__ __volatile__ (
    "lock cmpxchg16b %1\n"
    "setz %0"
    : "=q" (success), "+m" (sourceOperand), "+a" (lowerCompare), "+d" (upperCompare)
    : "b" (newLowerValue), "c" (newUpperValue)
    : "cc" );

    return success;
}

  int ff = 6;

LockFreeQueueNode *allocateAndInitializeLockFreeQueueNode(void *data) {

    LockFreeQueueNode *output = malloc(sizeof(LockFreeQueueNode));
    if (output != NULL) {

        output->data = data;
        output->next = &ff;
    }

    return output;
}

LockFreeQueue *allocateAndInitializeLockFreeQueue(long maxMessageSize, long maxStorageSize) {

    LockFreeQueue *output;
    LockFreeQueueNode *firstNode;

    output = malloc(sizeof(LockFreeQueueNode));
    if (output != NULL) {

        firstNode = allocateAndInitializeLockFreeQueueNode(NULL);
        if (firstNode != NULL) {

            output->maxStorageSize = maxStorageSize;
            output->maxMessageSize = maxMessageSize;
            output->tailNode = firstNode;
            output->headNode = firstNode;
            output->currentUsedStorage = 0;

        } else {

            free(output);
            output = NULL;
        }
    }

    return output;
}


unsigned int enqueue(LockFreeQueue *queue, void *data) {

    LockFreeQueueNode *newNode;
    LockFreeQueueNode *actualTailNode;
    LockFreeQueueNode *nodeAfterTailNode;
    DCASOperand doubleCASOperand;

    unsigned long currentUsedStorage = ADD_AND_FETCH(&queue->currentUsedStorage, 1);
    if (currentUsedStorage > queue->maxStorageSize) {
        ADD_AND_FETCH(&queue->currentUsedStorage, -1);
        return NOT_ENOUGH_STORAGE_AVAILABLE;
    }

    newNode = allocateAndInitializeLockFreeQueueNode(data);
    if (newNode == NULL)
        return ALLOCATION_FAILED;

    while (1) {

        nodeAfterTailNode = queue->tailNode->next;
        actualTailNode = queue->tailNode;

        doubleCASOperand.lo = &ff;
        doubleCASOperand.hi = actualTailNode;

        if (CAS2(doubleCASOperand, &ff, queue->tailNode, newNode, newNode))
            break;
    }

    return SUCCESS;
}

void *dequeue(LockFreeQueue *queue) {

    void *output;

    LockFreeQueueNode *targetNode;
    LockFreeQueueNode *nodeAfterHeadNode;

    long currentUsedStorage = ADD_AND_FETCH(&queue->currentUsedStorage, -1);
    if (currentUsedStorage < 0) {
        ADD_AND_FETCH(&queue->currentUsedStorage, 1);
        return NULL;
    }

    while (1) {

        nodeAfterHeadNode = queue->headNode->next;
        targetNode = queue->headNode;

        if(COMPARE_AND_SWAP(&(queue->headNode), &targetNode, nodeAfterHeadNode) == SUCCESS)
            break;
    }

    output = targetNode->data;
    free(targetNode);

    return output;
}
