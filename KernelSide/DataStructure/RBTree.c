#include "RBTree.h"

#include "../Common/BasicDefines.h"

#include <linux/rbtree.h>
#include <linux/kernel.h>
#include <linux/slab.h>

typedef struct {

    unsigned int id;
    struct rb_node node;

    void *data;

} RBTreeNodeContent;

RBTreeNodeContent *allocateRBTreeNodeContent(unsigned int id, void *data) {

    RBTreeNodeContent *output = kmalloc(sizeof(RBTreeNodeContent), GFP_KERNEL);
    if (output != NULL) {

        output->id = id;
        output->data = data;
    }

    return output;
}


RBTree *allocateRBTree(void) {
    return kmalloc(sizeof(RBTree), GFP_KERNEL);
}

int insertRBTree(RBTree *input, unsigned int id, void *data) {

    RBTreeNodeContent *nodeContent = allocateRBTreeNodeContent(id, data);
    if (nodeContent == NULL)
        return FAILURE;
    else {

        RBTreeNodeContent *currentNodeContent = NULL;

        struct rb_node **new = &(input->rb_node);
        struct rb_node *parent = NULL;

        while (*new) {

            currentNodeContent = container_of(*new, RBTreeNodeContent, node);
            parent = *new;

            if (nodeContent->id < currentNodeContent->id)
                new = &((*new)->rb_left);
            else if (nodeContent->id > currentNodeContent->id)
                new = &((*new)->rb_right);
            else {

                kfree(nodeContent);
                return FAILURE;
            }
        }

        rb_link_node(&nodeContent->node, parent, new);
        rb_insert_color(&nodeContent->node, input);

        return SUCCESS;
    }
}

void *searchRBTree(RBTree *input, unsigned int id) {

    void *output = NULL;
    struct rb_node *currentNode;

    currentNode = input->rb_node;

    while (currentNode != NULL) {

        RBTreeNodeContent *currentNodeContent = container_of(currentNode, RBTreeNodeContent, node);

        if (id < currentNodeContent->id)
            currentNode = currentNode->rb_left;
        else if (id > currentNodeContent->id)
            currentNode = currentNode->rb_right;
        else {
            output = currentNodeContent->data;
            break;
        }

    }

    return output;
}

RBTree *copyRBTree(RBTree *input) {

    RBTree* newRBTree = allocateRBTree();

    struct rb_node *currentNode;
    for (currentNode = rb_first(input); currentNode; currentNode = rb_next(currentNode)) {

        RBTreeNodeContent *currentNodeContent = container_of(currentNode, RBTreeNodeContent, node);
        insertRBTree(newRBTree, currentNodeContent->id, currentNodeContent->data);
    }

    return newRBTree;
}

void freeRBTree(RBTree *input, void (*dataFreeFunction)(void *)) {

    RBTreeNodeContent *currentNodeContent;
    struct rb_node *currentNode;
    struct rb_node *nextNode;

    for (currentNode = rb_first(input); currentNode; ) {

        nextNode = rb_next(currentNode);
        currentNodeContent = container_of(currentNode, RBTreeNodeContent, node);

        if (dataFreeFunction != NULL)
            (*dataFreeFunction)(currentNodeContent->data);

        kfree(currentNodeContent);
        kfree(currentNode);

        currentNode = nextNode;
    }

    kfree(input);
}


void freeRBTreeContentExcluded(RBTree *input) {
    freeRBTree(input, NULL);
}

void freeRBTreeContentIncluded(RBTree *input, void (*dataFreeFunction)(void *)) {
    freeRBTree(input, dataFreeFunction);
}