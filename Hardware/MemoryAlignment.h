#pragma once

#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

//#define PAGE_SIZE 4096
#define CACHE_LINE_SIZE 64
#define ALIGNED_MEMORY __attribute__((aligned(CACHE_LINE_SIZE)))

inline void *allocateAlignedMemoryArea(size_t alignment, size_t size) {
    void *output;

    int returnValue = posix_memalign(&output, alignment, size);
    if (returnValue != 0) {

        fprintf(stderr, "[ERROR] %s", strerror(returnValue));
        exit(EXIT_FAILURE);

    } else
        return output;
}