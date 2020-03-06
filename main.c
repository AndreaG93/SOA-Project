//
// Created by andrea on 06/03/20.
//

#include <stdio.h>


inline unsigned long getPendingFromStatus(unsigned long input) {
    return (input & 1);
}

inline unsigned long getIdentifierFromStatus(unsigned long input) {
    return (input >> 1);
}

#define GetS(data, index, size) (((data) & GETMASK((index), (size))) >> (index))

int main() {

    unsigned long dd = 3;
    unsigned long status = (dd & 1);
    unsigned long see = (dd >> 1);

    fprintf(stderr, "%lu\n", status);
    fprintf(stderr, "%lu\n", see);

    return 6;
}