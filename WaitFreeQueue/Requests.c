#include "Requests.h"

inline unsigned long getPendingBitFlagFromRequestState(RequestState input) {
    return (input & 1);
}

inline unsigned long getIdentifierFromRequestState(RequestState input) {
    return (input >> 1);
}