#include "SemiLockFreeQueue.h"
#include "../Common/BasicDefines.h"
#include "../Common/ModuleMetadata.h"
#include "../Common/SynchronizationPrimitives.h"

#include <linux/slab.h>
#include <linux/kobject.h>

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

SemiLockFreeQueue *allocateSemiLockFreeQueue(long maxMessageSize, long maxStorageSize, struct kobject *kObject) {

    SemiLockFreeQueueNode *dummyNode;
    SemiLockFreeQueue *output;

    output = kmalloc(sizeof(SemiLockFreeQueue), GFP_KERNEL);
    if (output != NULL) {

        dummyNode = allocateSemiLockFreeQueueNode(NULL);
        if (dummyNode != NULL) {

            output->head = dummyNode;
            output->tail = dummyNode;
            output->maxMessageSize = maxMessageSize;
            output->maxStorageSize = maxStorageSize;
            output->kObject = kObject;

        } else {

            kfree(output);
            output = NULL;
        }
    }

    return output;
}

void freeSemiLockFreeQueue(SemiLockFreeQueue *queue, void (*dataFreeFunction)(void *)) {

    SemiLockFreeQueueNode *currentNode;
    SemiLockFreeQueueNode *nextNode;

    for (currentNode = queue->head; currentNode != NULL;) {

        nextNode = currentNode->next;

        if (currentNode->data != NULL)
            (*dataFreeFunction)(currentNode->data);

        kfree(currentNode);
        currentNode = nextNode;
    }

    kfree(queue);
}

unsigned char enqueue(SemiLockFreeQueue *queue, void *data) {

    SemiLockFreeQueueNode *actualTail;
    SemiLockFreeQueueNode *newNode = allocateSemiLockFreeQueueNode(data);

    unsigned long currentUsedStorage = ADD_AND_FETCH(&queue->currentUsedStorage, 1);
    if (currentUsedStorage > queue->maxStorageSize) {
        SUB_AND_FETCH(&queue->currentUsedStorage, 1);
        return FAILURE;
    }

    while (TRUE) {

        actualTail = queue->tail;
        if (__sync_bool_compare_and_swap(&queue->tail, actualTail, newNode)) {
            actualTail->next = newNode;
            break;
        }
    }

    printk("'%s': 'SemiLockFreeQueue': current messages: %ld!\n", MODULE_NAME, queue->currentUsedStorage);

    return SUCCESS;
}

void *dequeue(SemiLockFreeQueue *queue) {

    SemiLockFreeQueueNode *actualHead;
    void *output = NULL;

    unsigned long currentUsedStorage = SUB_AND_FETCH(&queue->currentUsedStorage, 1);
    if (currentUsedStorage < 0) {
        ADD_AND_FETCH(&queue->currentUsedStorage, 1);
        return NULL;
    }

    do {
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

    printk("'%s': 'SemiLockFreeQueue': current messages: %ld!\n", MODULE_NAME, queue->currentUsedStorage);

    return output;
}