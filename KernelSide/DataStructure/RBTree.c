#include "RBTree.h"

#include "../Common/BasicDefines.h"
#include "../Common/ModuleMetadata.h"

#include <linux/rbtree.h>
#include <linux/kernel.h>
#include <linux/slab.h>

typedef struct {

    struct rb_node node;

    unsigned long nodeID;
    void *data;

} RBTreeNodeContent;


RBTree *allocateRBTree(void) {
    return kmalloc(sizeof(RBTree), GFP_KERNEL);
}

RBTreeNodeContent *allocateRBTreeNodeContent(unsigned long nodeID, void *data) {

    RBTreeNodeContent *output = kmalloc(sizeof(RBTreeNodeContent), GFP_KERNEL);
    if (output != NULL) {

        output->nodeID = nodeID;
        output->data = data;
    }

    return output;
}

DriverError insertRBTree(RBTree *input, unsigned long nodeID, void *data) {

    RBTreeNodeContent *nodeContent = allocateRBTreeNodeContent(nodeID, data);
    if (nodeContent == NULL)
        return FAILURE;
    else {

        RBTreeNodeContent *currentNodeContent = NULL;

        struct rb_node **new = &(input->rb_node);
        struct rb_node *parent = NULL;

        while (*new) {

            currentNodeContent = container_of(*new, RBTreeNodeContent, node);
            parent = *new;

            if (nodeContent->nodeID < currentNodeContent->nodeID)
                new = &((*new)->rb_left);
            else if (nodeContent->nodeID > currentNodeContent->nodeID)
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

void *searchRBTree(RBTree *input, unsigned long nodeID) {

    void *output = NULL;
    struct rb_node *currentNode;

    currentNode = input->rb_node;

    while (currentNode != NULL) {

        RBTreeNodeContent *currentNodeContent = container_of(currentNode, RBTreeNodeContent, node);

        if (nodeID < currentNodeContent->nodeID)
            currentNode = currentNode->rb_left;
        else if (nodeID > currentNodeContent->nodeID)
            currentNode = currentNode->rb_right;
        else {
            output = currentNodeContent->data;
            break;
        }

    }

    return output;
}

void performFunctionRBTree(RBTree *input, void (*function_A)(void *), void (*function_B)(void *)) {

    struct rb_node *currentNode;
    for (currentNode = rb_first(input); currentNode; currentNode = rb_next(currentNode)) {

        RBTreeNodeContent *currentNodeContent = container_of(currentNode, RBTreeNodeContent, node);
        (*function_A)(currentNodeContent->data);
        (*function_B)(currentNodeContent->data);
    }
}

DriverError removeRBTree(RBTree *input, unsigned long nodeID, void (*freeFunction)(void *)) {

    struct rb_node *currentNode;

    currentNode = input->rb_node;

    while (currentNode != NULL) {

        RBTreeNodeContent *currentNodeContent = container_of(currentNode, RBTreeNodeContent, node);

        if (nodeID < currentNodeContent->nodeID)
            currentNode = currentNode->rb_left;
        else if (nodeID > currentNodeContent->nodeID)
            currentNode = currentNode->rb_right;
        else {

            if (freeFunction != NULL)
                (*freeFunction)(currentNodeContent->data);

            rb_erase(&currentNodeContent->node, input);

            return SUCCESS;
        }
    }

    return ERR_NOT_FOUND;
}

RBTree *copyRBTree(RBTree *input) {

    RBTree *newRBTree = allocateRBTree();

    struct rb_node *currentNode;
    for (currentNode = rb_first(input); currentNode; currentNode = rb_next(currentNode)) {

        RBTreeNodeContent *currentNodeContent = container_of(currentNode, RBTreeNodeContent, node);
        insertRBTree(newRBTree, currentNodeContent->nodeID, currentNodeContent->data);
    }

    return newRBTree;
}

void cleanRBTree(RBTree *input, void (*freeFunction)(void *)) {

    RBTreeNodeContent *currentNodeContent;
    struct rb_node *currentNode;

    while (TRUE) {

        currentNode = rb_first(input);

        if (currentNode == NULL)
            break;
        else {

            currentNodeContent = container_of(currentNode, RBTreeNodeContent, node);
            if (freeFunction != NULL)
                (*freeFunction)(currentNodeContent->data);

            rb_erase(&currentNodeContent->node, input);
        }
    }
}

void freeRBTreeContentExcluded(RBTree *input) {
    cleanRBTree(input, NULL);
    kfree(input);
}

void freeRBTreeContentIncluded(RBTree *input, void (*freeFunction)(void *)) {
    cleanRBTree(input, freeFunction);
    kfree(input);
}