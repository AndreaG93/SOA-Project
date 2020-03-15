#pragma once

#include <linux/rbtree.h>

typedef struct rb_root RBTree;

RBTree* allocateRBTree(void);

void freeRBTree(RBTree* input);

void freeRBTreeContentIncluded(RBTree *input, void (*dataFreeFunction)(void *));

int insertRBTree(RBTree *input, unsigned int id, void *data);

void *searchRBTree(RBTree *input, unsigned int id);

RBTree* copyRBTree(RBTree *input);