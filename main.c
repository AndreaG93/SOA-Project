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