#include "RedBlackTree.h"

#include <linux/rbtree.h>
#include <linux/kernel.h>
#include <linux/slab.h>

typedef struct {

    unsigned int id;
    struct rb_node node;

    void *data;

} NodeContent __attribute__((aligned(sizeof(long))));

RCURedBlackTree *cloneRCURedBlackTree(void) {
    return NULL;
}

NodeContent *allocateNodeData(unsigned int id, void *data) {

    NodeContent *output = kmalloc(sizeof(NodeContent), GFP_KERNEL);

    if (output == NULL)
        printk("'%s': 'NodeContent' allocation FAILED!\n", "DSAdsasfasdfds");  // TODO CORRECT PLEASE
    else {
        output->id = id;
        output->data = data;
    }

    return output;
}

void insert(RCURedBlackTree *root, unsigned int id, void *data) {

    NodeContent *nodeContent = allocateNodeData(id, data);
    NodeContent *currentNodeContent = NULL;

    struct rb_node **new = &(root->rb_node);
    struct rb_node *parent = NULL;

    while (*new) {

        currentNodeContent = container_of(*new, NodeContent, node);
        parent = *new;

        if (nodeContent->id < currentNodeContent->id)
            new = &((*new)->rb_left);
        else
            new = &((*new)->rb_right);
    }

    rb_link_node(&nodeContent->node, parent, new);
    rb_insert_color(&nodeContent->node, root);
}

void *search(RCURedBlackTree *root, unsigned int id) {

    void *output = NULL;
    struct rb_node *currentNode;

    currentNode = root->rb_node;

    while (currentNode != NULL) {

        NodeContent *currentNodeContent = container_of(currentNode, NodeContent, node);

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

void delete(RCURedBlackTree *root, unsigned int id) {
/*
    NodeContent *targetNodeContent = search(root, id);

    if (targetNodeContent != NULL) {
        rb_erase(&targetNodeContent->node, root);
        kfree(targetNodeContent);
    }
    */
}

void atomicallySwap(RCURedBlackTree *root, unsigned int id, void *newData) {

    struct rb_node *currentNode;

    currentNode = root->rb_node;

    while (currentNode != NULL) {

        NodeContent *currentNodeContent = container_of(currentNode, NodeContent, node);

        if (id < currentNodeContent->id)
            currentNode = currentNode->rb_left;
        else if (id > currentNodeContent->id)
            currentNode = currentNode->rb_right;
        else {
            void *oldData = currentNodeContent->data;
            while (__sync_bool_compare_and_swap(&currentNodeContent->data, oldData, newData) != SUCCESS);
            return;
        }

    }
}

