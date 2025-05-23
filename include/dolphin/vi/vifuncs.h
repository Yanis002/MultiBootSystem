#ifndef _DOLPHIN_VIFUNCS_H_
#define _DOLPHIN_VIFUNCS_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "dolphin/gx/GXStruct.h"
#include "dolphin/types.h"
#include "dolphin/vi/vitypes.h"

void VIInit(void);
void VIConfigure(GXRenderModeObj* rm);
VIRetraceCallback VISetPostRetraceCallback(VIRetraceCallback callback);
void VIFlush(void);
u32 VIGetTvFormat(void);
void VISetNextFrameBuffer(void* fb);
void VIWaitForRetrace(void);
void VISetBlack(BOOL black);
u32 VIGetNextField(void);
u32 VIGetCurrentLine(void);
void __VIGetCurrentPosition(s16* x, s16* y);
VIRetraceCallback VISetPreRetraceCallback(VIRetraceCallback cb);

#ifdef __cplusplus
};
#endif

#endif
