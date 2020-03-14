#include "SemiLockFreeQueue.h"
#include <linux/slab.h>
#include <linux/kobject.h>

SemiLockFreeQueueNode *allocateAndInitializeSemiLockFreeQueueNode(void *data) {

    SemiLockFreeQueueNode *output = kmalloc(sizeof(SemiLockFreeQueueNode), GFP_KERNEL);
    if (output != NULL) {

        output->data = data;
        output->next = NULL;
    }

    return output;
}

SemiLockFreeQueue *allocateAndInitializeSemiLockFreeQueue(long maxMessageSize, long maxStorageSize, struct kobject *kObject) {

    SemiLockFreeQueueNode *dummyNode;
    SemiLockFreeQueue *output;

    output = kmalloc(sizeof(SemiLockFreeQueue), GFP_KERNEL);
    if (output != NULL) {

        dummyNode = allocateAndInitializeSemiLockFreeQueueNode(NULL);
        if (dummyNode != NULL) {

            output->head = dummyNode;
            output->tail = dummyNode;
            output->kObject = kObject;

        } else {

            free(output);
            output = NULL;
        }
    }

    return output;
}

void freeSemiLockFreeQueue(SemiLockFreeQueue *queue) {

    SemiLockFreeQueueNode *nextNode;

    while (queue->head != NULL) {

        nextNode = queue->head->next;

        if (queue->head->data != NULL)
            free(queue->head->data);
        free(queue->head);

        queue->head = nextNode;
    }

    free(queue);
}

unsigned int enqueue(SemiLockFreeQueue *queue, void *data) {

    SemiLockFreeQueueNode *actualTail;
    SemiLockFreeQueueNode *newNode = allocateAndInitializeSemiLockFreeQueueNode(data);

    unsigned long currentUsedStorage = __sync_add_and_fetch(&queue->currentUsedStorage, 1);
    if (currentUsedStorage > queue->maxStorageSize) {
        __sync_sub_and_fetch(&queue->currentUsedStorage, 1);
        return 1;
    }

    do {
        actualTail = queue->tail;
        if (__sync_bool_compare_and_swap(&queue->tail, actualTail, newNode)) {
            actualTail->next = newNode;
            break;
        }
    } while (1);

    return 0;
}

void *dequeue(SemiLockFreeQueue *queue) {

    SemiLockFreeQueueNode *actualHead;
    void *output = NULL;

    unsigned long currentUsedStorage = __sync_sub_and_fetch(&queue->currentUsedStorage, 1);
    if (currentUsedStorage < 0) {
        __sync_add_and_fetch(&queue->currentUsedStorage, 1);
        return NULL;
    }

    do {
        actualHead = queue->head;
    } while (actualHead == NULL || !__sync_bool_compare_and_swap(&queue->head, actualHead, NULL));

    if (actualHead->next == NULL) {
        queue->head = actualHead;
        return NULL;
    }

    output = ((SemiLockFreeQueueNode *) actualHead->next)->data;
    queue->head = actualHead->next;
    free(actualHead);

    return output;
}