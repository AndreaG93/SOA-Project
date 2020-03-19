#include <linux/slab.h>
#include <linux/kobject.h>

#include "SemiLockFreeQueue.h"
#include "../Common/BasicDefines.h"
#include "../Common/SynchronizationPrimitives.h"

typedef struct {

    void *next;
    void *data;
    unsigned long dataSize;

} SemiLockFreeQueueNode;

SemiLockFreeQueueNode *allocateSemiLockFreeQueueNode(void *data) {

    SemiLockFreeQueueNode *output = kmalloc(sizeof(SemiLockFreeQueueNode), GFP_KERNEL);
    if (output != NULL) {

        output->data = data;
        output->next = NULL;
    }

    return output;
}

SemiLockFreeQueue *allocateSemiLockFreeQueue(long maxMessageSize, long maxStorageSize) {

    SemiLockFreeQueue *output;
    SemiLockFreeQueueNode *outputDummyNode;

    output = kmalloc(sizeof(SemiLockFreeQueue), GFP_KERNEL);
    if (output == NULL)
        return NULL;

    outputDummyNode = allocateSemiLockFreeQueueNode(NULL);
    if (outputDummyNode == NULL) {

        kfree(output);
        return NULL;
    }

    output->head = outputDummyNode;
    output->tail = outputDummyNode;
    output->maxMessageSize = maxMessageSize;
    output->maxStorageSize = maxStorageSize;

    return output;
}

void freeSemiLockFreeQueue(SemiLockFreeQueue *queue, void (*freeFunction)(void *)) {

    SemiLockFreeQueueNode *currentNode;
    SemiLockFreeQueueNode *nextNode;

    for (currentNode = queue->head; currentNode != NULL;) {

        nextNode = currentNode->next;

        if (currentNode->data != NULL)
            (*freeFunction)(currentNode->data);

        kfree(currentNode);
        currentNode = nextNode;
    }

    kfree(queue);
}

DriverError enqueue(SemiLockFreeQueue *queue, void *data, unsigned long dataSize) {

    SemiLockFreeQueueNode *actualTail;
    SemiLockFreeQueueNode *newNode;
    unsigned long currentUsedStorage;

    currentUsedStorage = ADD_AND_FETCH(&queue->currentUsedStorage, dataSize);
    if (currentUsedStorage > queue->maxStorageSize) {
        SUB_AND_FETCH(&queue->currentUsedStorage, dataSize);
        return FAILURE;
    }

    newNode = allocateSemiLockFreeQueueNode(data);

    while (TRUE) {

        actualTail = queue->tail;
        if (__sync_bool_compare_and_swap(&queue->tail, actualTail, newNode)) {
            actualTail->next = newNode;
            break;
        }
    }

    return SUCCESS;
}

void *dequeue(SemiLockFreeQueue *queue, unsigned long (*getSizeFromData)(void *)) {

    SemiLockFreeQueueNode *actualHead;
    void *output;
    unsigned long outputSize;
    unsigned long currentUsedStorage;

    do {

        currentUsedStorage = ADD_AND_FETCH(&queue->currentUsedStorage, 0);
        if (currentUsedStorage == 0)
            return NULL;

        actualHead = queue->head;

    } while (actualHead == NULL || !COMPARE_AND_SWAP(&queue->head, actualHead, NULL));

    if (actualHead->next == NULL) {
        queue->head = actualHead;
        return NULL;
    }

    output = ((SemiLockFreeQueueNode *) actualHead->next)->data;
    ((SemiLockFreeQueueNode *) actualHead->next)->data = NULL;

    queue->head = actualHead->next;
    kfree(actualHead);

    outputSize = (*getSizeFromData)(output);
    SUB_AND_FETCH(&queue->currentUsedStorage, outputSize)

    return output;
}