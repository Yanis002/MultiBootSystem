#ifndef _DOLPHIN_DBINTERFACE_H_
#define _DOLPHIN_DBINTERFACE_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "dolphin/types.h"

#define OS_DBINTERFACE_ADDR 0x00000040

typedef struct DBInterface {
    u32 bPresent;
    u32 exceptionMask;
    void (*ExceptionDestination)(void);
    void* exceptionReturn;
} DBInterface;

extern DBInterface* __DBInterface;

void DBInit(void);
void DBInitComm(int* inputFlagPtr, int* mtrCallback);
void DBPrintf(char* format, ...);

#ifdef __cplusplus
};
#endif

#endif
