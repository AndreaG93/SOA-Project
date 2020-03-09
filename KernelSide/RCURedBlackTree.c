#include "RCURedBlackTree.h"
#include <linux/rbtree.h>

void insert(RCURedBlackTree *root, TreeNodeContent *data)
{
    struct rb_node **new = &(root->rb_node), *parent = NULL;

    while (*new) {
        RBTreeNodeContent *this = container_of(*new, RBTreeNodeContent, node);

        parent = *new;
        if (data->deviceFileMinorNumber < this->deviceFileMinorNumber)
            new = &((*new)->rb_left);
        else
            new = &((*new)->rb_right);
    }

    rb_link_node(&data->node, parent, new);
    rb_insert_color(&data->node, root);
}

TreeNodeContent *search(RCURedBlackTree *root, unsigned int deviceFileMinorNumber)
{
    struct rb_node *node = root->rb_node;

    while (node) {
        TreeNodeContent *data = container_of(node, struct mytype, node);

        if (data->deviceFileMinorNumber < this->deviceFileMinorNumber)
            node = node->rb_left;
        else if (data->deviceFileMinorNumber > this->deviceFileMinorNumber)
            node = node->rb_right;
        else
            return data;
    }
    return NULL;
}