#pragma once

#include <linux/rbtree.h>

#include "../Common/BasicDefines.h"

typedef struct rb_root RBTree;

RBTree* allocateRBTree(void);

DriverError insertRBTree(RBTree *input, unsigned long nodeID, void *data);

void *searchRBTree(RBTree *input, unsigned long nodeID);

DriverError removeRBTree(RBTree *input, unsigned long nodeID, void (*freeFunction)(void *));

void cleanRBTree(RBTree *input, void (*freeFunction)(void *));

void freeRBTreeContentExcluded(RBTree *input);

void freeRBTreeContentIncluded(RBTree *input, void (*freeFunction)(void *));

RBTree* copyRBTree(RBTree *input);

void performFunctionRBTree(RBTree *input, void (*function_A)(void *), void (*function_B)(void *));