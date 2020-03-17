#pragma once

typedef struct {

    unsigned int sessionCommand;
    unsigned long sessionCommandParameter;

} SessionData;

SessionData* allocateSessionData(unsigned int sessionCommand, unsigned long sessionCommandParameter);

void freeSessionData(SessionData* input);