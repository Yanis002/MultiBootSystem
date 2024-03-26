#ifndef _DOLPHIN_DBINTERFACE_H_
#define _DOLPHIN_DBINTERFACE_H_

#define OS_DBINTERFACE_ADDR 0x00000040

#include "dolphin/types.h"

typedef struct DBInterface {
    u32 bPresent;
    u32 exceptionMask;
    void (*ExceptionDestination)(void);
    void* exceptionReturn;
} DBInterface;

extern DBInterface* __DBInterface;

void DBInit(void);
void DBInitComm(int* inputFlagPtr, int* mtrCallback);
static void __DBExceptionDestination(void);
void DBPrintf(char* format, ...);

#endif
