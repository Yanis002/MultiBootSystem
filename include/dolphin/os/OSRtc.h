#ifndef _DOLPHIN_OS_OSRTC_H_
#define _DOLPHIN_OS_OSRTC_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "dolphin/types.h"

BOOL WriteSram(void* buffer, u32 offset, u32 size);
void __OSInitSram(void);
BOOL __OSUnlockSram(BOOL commit);
BOOL __OSUnlockSramEx(BOOL commit);
BOOL __OSSyncSram(void);
u32 OSGetSoundMode(void);
void OSSetSoundMode(u32 mode);
u32 OSGetProgressiveMode(void);
void OSSetProgressiveMode(u32 mode);
u16 OSGetWirelessID(s32 channel);
void OSSetWirelessID(s32 channel, u16 id);
u16 OSGetGbsMode(void);
void OSSetGbsMode(u16 mode);

#ifdef __cplusplus
};
#endif

#endif
