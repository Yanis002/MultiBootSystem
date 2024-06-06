#ifndef _DOLPHIN_GX_MISC_H
#define _DOLPHIN_GX_MISC_H

#ifdef __cplusplus
extern "C" {
#endif

#include "dolphin/gx/GXFifo.h"
#include "dolphin/types.h"

GXFifoObj* GXInit(void* base, u32 size);
void __GXInitGX(void);
void __GXPEInit(void);
void __GXAbort(void);

#ifdef __cplusplus
};
#endif

#endif
