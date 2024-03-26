#ifndef _DOLPHIN_VIFUNCS_H_
#define _DOLPHIN_VIFUNCS_H_

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

VIRetraceCallback VISetPreRetraceCallback(VIRetraceCallback callback);
void __VIGetCurrentPosition(s16* x, s16* y);
u32 VIGetDTVStatus(void);

#ifdef TARGET_PC
void VISetWindowTitle(const char* title);
void VISetWindowFullscreen(bool fullscreen);
bool VIGetWindowFullscreen();
#endif

#endif
