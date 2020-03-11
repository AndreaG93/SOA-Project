#include "Requests.h"
#include "../KernelSide/BasicOperations/BasicDefines.h"
#include "../KernelSide/BasicOperations/BitManipulation.h"

inline unsigned long getPendingBitFlagFromRequestState(RequestState input) {
    return (input & 1);
}

inline unsigned long getIdentifierFromRequestState(RequestState input) {
    return (input >> 1);
}

void setRequestState(RequestState *input, unsigned long requestState, unsigned long cellID) {
    *input = (*input & 1) | ((cellID << 1UL) & ~1UL);
}