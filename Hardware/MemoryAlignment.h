#pragma once

#include <stddef.h>

#define PAGE_SIZE 4096
#define CACHE_LINE_SIZE 64
#define ALIGNED_MEMORY __attribute__((aligned(CACHE_LINE_SIZE)))

inline void *allocateAlignedMemoryArea(size_t alignment, size_t size);