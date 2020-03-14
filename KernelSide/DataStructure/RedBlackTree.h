// For references: https://github.com/torvalds/linux/blob/master/Documentation/rbtree.txt

#pragma once

#include <linux/rbtree.h>

typedef struct rb_root RedBlackTree;

RedBlackTree* allocateRedBlackTree(void);

void freeRedBlackTree(RedBlackTree* input);

void redBlackTreeInsert(RedBlackTree *input, unsigned int id, void *data);

void *redBlackTreeSearch(RedBlackTree *input, unsigned int id);

void redBlackTreeDelete(RedBlackTree *input, unsigned int id);

//void redBlackTreeElementSwap(RedBlackTree *input, unsigned int id, void *newData);