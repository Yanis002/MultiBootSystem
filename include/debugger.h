#ifndef DEBUGGER_H
#define DEBUGGER_H

#ifdef __cplusplus
extern "C" {
#endif

#include "dolphin.h"

// EXI error codes
typedef enum {
    AMC_EXI_NO_ERROR = 0,
    AMC_EXI_UNSELECTED = 1
} AmcExiError;

void EXI2_Init(void);
void EXI2_EnableInterrupts(void);
BOOL EXI2_Poll(void);
AmcExiError EXI2_ReadN(void);
AmcExiError EXI2_WriteN(void);
void EXI2_Reserve(void);
void EXI2_Unreserve(void);
BOOL AMC_IsStub(void);

BOOL Hu_IsStub(void);

#ifdef __cplusplus
};
#endif

#endif
