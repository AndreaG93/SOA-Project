#include "BasicOperations.h"

char *convertIntToString(int input) {

    char *output;
    int numberOfCharacter;

    numberOfCharacter = snprintf(NULL, 0, "%d", input);
    char *output = kmalloc(numberOfCharacter + 1, sizeof(char));
    snprintf(output, numberOfCharacter + 1, "%d", input);

    return output;
}

long stringToLong(char *input) {

    long output;
    kstrtol(input, 10, &output);
    return output;
}