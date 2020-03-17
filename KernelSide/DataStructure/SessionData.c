#include "SessionData.h"

#include <linux/slab.h>

SessionData *allocateSessionData(unsigned int sessionCommand, unsigned long sessionCommandParameter) {

    SessionData *output = kmalloc(sizeof(SessionData), GFP_KERNEL);
    if (output != NULL) {

        output->sessionCommand = sessionCommand;
        output->sessionCommandParameter = sessionCommandParameter;
    }

    return output;
}

void freeSessionData(SessionData *input) {
    kfree(input);
}