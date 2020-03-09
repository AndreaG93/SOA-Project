// For references: https://github.com/torvalds/linux/blob/master/Documentation/rbtree.txt

#pragma once

#include <linux/rbtree.h>

typedef RCURedBlackTree struct rb_root;
typedef RCURedBlackTreeNode struct rb_node;

typedef struct {

    unsigned int deviceFileMinorNumber;
    RCURedBlackTreeNode node;
    char *string;           // TODO must became wait free queue!!!!

} TreeNodeContent;
__attribute__((aligned(sizeof(long))));


RCURedBlackTree *allocateRCURedBlackTree(void);

void freeRCURedBlackTree(RCURedBlackTree *root);

void insert(RCURedBlackTree *root, TreeNodeContent *data);

TreeNodeContent *search(RCURedBlackTree *root, unsigned int deviceFileMinorNumber);

void delete(RCURedBlackTree *root, unsigned int deviceFileMinorNumber);