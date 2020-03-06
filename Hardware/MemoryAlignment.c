#include <stdlib.h>
#include <string.h>
#include <stdio.h>

inline void *allocateAlignedMemoryArea(size_t alignment, size_t size) {
    void *output;

    int returnValue = posix_memalign(&output, alignment, size);
    if (returnValue != 0) {

        fprintf(stderr, "[ERROR] %s", strerror(returnValue));
        exit(EXIT_FAILURE);

    } else
        return output;
}