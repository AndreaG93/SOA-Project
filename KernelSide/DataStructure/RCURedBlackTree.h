// For references: https://github.com/torvalds/linux/blob/master/Documentation/rbtree.txt

#pragma once

#include <linux/rbtree.h>

typedef struct rb_root RCURedBlackTree;

void insert(RCURedBlackTree *root, unsigned int id, void *data);

void *search(RCURedBlackTree *root, unsigned int id);

void delete(RCURedBlackTree *root, unsigned int id);

void atomicallySwap(RCURedBlackTree *root, unsigned int id, void *newData);