#include "BasicOperations.h"
#include <linux/kernel.h>
#include <linux/slab.h>

long stringToLong(const char *input) {

    long output;
    kstrtol(input, 10, &output);
    return output;
}

char *convertIntToString(int input) {

    char *output;
    int numberOfCharacter;

    numberOfCharacter = snprintf(NULL, 0, "%d", input);
    output = kmalloc(numberOfCharacter + 1, sizeof(char));
    snprintf(output, numberOfCharacter + 1, "%d", input);

    return output;
}