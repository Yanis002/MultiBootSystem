#ifndef OSDSP_TASK_H
#define OSDSP_TASK_H

#include "dolphin/dsp.h"
#include "dolphin/types.h"

typedef struct OSContext OSContext;

extern DSPTaskInfo* DSP_prior_task;

extern "C" void __DSPHandler(int, OSContext*);
void DsyncFrame2(u32, u32, u32);
void Dsp_Update_Request();
BOOL Dsp_Running_Check();
void Dsp_Running_Start();

#endif /* OSDSP_TASK_H */
