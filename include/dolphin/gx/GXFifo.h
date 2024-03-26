#ifndef _DOLPHIN_GX_GXFIFO_H_
#define _DOLPHIN_GX_GXFIFO_H_

#include "dolphin/gx/GXEnum.h"

#define GX_WRITE_U8(val)  (GXWGFifo.u8 = val)
#define GX_WRITE_U16(val) (GXWGFifo.u16 = val)
#define GX_WRITE_U32(val) (GXWGFifo.u32 = (u32)val)
#define GX_WRITE_F32(val) (GXWGFifo.f32 = (f32)val)

typedef void (*GXBreakPtCallback)(void);

typedef struct {
    u8 pad[128];
} GXFifoObj;

typedef struct _GXFifoObjPriv {
	void* base;        // _00
	void* end;         // _04
	u32 size;          // _08
	u32 highWatermark; // _0C
	u32 lowWatermark;  // _10
	void* readPtr;     // _14
	void* writePtr;    // _18
	s32 rwDistance;    // _1C
	u8 _20[0x60];      // _20
} GXFifoObjPriv;

void GXInitFifoBase(GXFifoObj* fifo, void* base, u32 size);
void GXInitFifoPtrs(GXFifoObj* fifo, void* readPtr, void* writePtr);
GXFifoObj* GXGetCPUFifo(void);
GXFifoObj* GXGetGPFifo(void);
void GXSetCPUFifo(GXFifoObj* fifo);
void GXSetGPFifo(GXFifoObj* fifo);
void GXGetGPStatus(GXBool* overhi, GXBool* underlow, GXBool* readIdle, GXBool* cmdIdle, GXBool* brkpt);
void GXInitFifoLimits(GXFifoObj* fifo, u32 hiWaterMark, u32 loWaterMark);
void* GXGetFifoBase(const GXFifoObj* fifo);
u32 GXGetFifoSize(const GXFifoObj* fifo);
GXBreakPtCallback GXSetBreakPtCallback(GXBreakPtCallback cb);

void __GXSaveCPUFifoAux(GXFifoObj* obj);
void __GXFifoReadEnable();
void __GXFifoReadDisable();
void __GXFifoLink(u8);
void __GXWriteFifoIntEnable(u32, u32);
void __GXWriteFifoIntReset(u32, u32);

#endif
