#ifndef _DOLPHIN_GX_GXMANAGE_H_
#define _DOLPHIN_GX_GXMANAGE_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "dolphin/gx/GXFifo.h"

typedef void (*GXDrawSyncCallback)(u16 token);
typedef void (*GXDrawDoneCallback)(void);

GXFifoObj* GXInit(void* base, u32 size);
void GXSetMisc(GXMiscToken token, u32 val);
void GXFlush(void);
void GXAbortFrame(void);
void GXSetDrawSync(u16 token);
u16 GXReadDrawSync(void);
void GXSetDrawDone(void);
void GXDrawDone(void);
void GXPixModeSync(void);
GXDrawSyncCallback GXSetDrawSyncCallback(GXDrawSyncCallback cb);
GXDrawDoneCallback GXSetDrawDoneCallback(GXDrawDoneCallback cb);

#ifdef __cplusplus
};
#endif

#endif
