//
// Created by andrea on 06/03/20.
//

#include <stdio.h>
#include "UserSide/FileOperationsTests.h"

#define REQUEST_STATUS_PENDING_BIT 1


inline unsigned long getPendingFromStatus(unsigned long input) {
    return (input & 1);
}

inline unsigned long getIdentifierFromStatus(unsigned long input) {
    return (input >> 1);
}

#define GetS(data, index, size) (((data) & GETMASK((index), (size))) >> (index))

void copyBitsStartingFromSecondBit(unsigned long *dest, unsigned long source) {
    *dest = (*dest & 1) | ((source << 1UL) & ~1UL);
}

void setBit(unsigned long *input, unsigned int bitPosition) {
    *input = *input | (1UL << bitPosition);
}

void clearBit(unsigned long *input, unsigned int bitPosition) {
    *input = *input & ~(1UL << bitPosition);
}

unsigned long getBit(unsigned long input, unsigned int bitPosition) {
    return (input >> bitPosition) & 1;
}



int main() {

    readTest();

    /*
    unsigned long dd = 4;

    setBit(&dd, 0);


    fprintf(stderr, "%lu\n", getBit(dd, 0));
    fprintf(stderr, "%lu\n", getBit(dd, 1));
    fprintf(stderr, "%lu\n", getBit(dd, 2));
    fprintf(stderr, "%lu\n", getBit(dd, 3));
    fprintf(stderr, "%lu\n", getBit(dd, 4));
*/
    return 0;
}