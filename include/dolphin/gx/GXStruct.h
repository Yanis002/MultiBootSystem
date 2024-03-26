#ifndef _DOLPHIN_GX_GXSTRUCT_H_
#define _DOLPHIN_GX_GXSTRUCT_H_

#include "dolphin/types.h"
#include "dolphin/vi/vitypes.h"
#include "dolphin/gx/GXEnum.h"

#define GXCOLOR_AS_U32(color) (*((u32*)&(color)))

typedef struct _GXRenderModeObj {
    /* 0x00 */ VITVMode viTVmode;
    /* 0x04 */ u16 fbWidth;
    /* 0x06 */ u16 efbHeight;
    /* 0x08 */ u16 xfbHeight;
    /* 0x0A */ u16 viXOrigin;
    /* 0x0C */ u16 viYOrigin;
    /* 0x0E */ u16 viWidth;
    /* 0x10 */ u16 viHeight;
    /* 0x14 */ VIXFBMode xFBmode;
    /* 0x18 */ u8 field_rendering;
    /* 0x?? */ u8 aa;
    /* 0x?? */ u8 sample_pattern[12][2];
    /* 0x?? */ u8 vfilter[7];
} GXRenderModeObj;

typedef struct _GXColor {
    u8 r;
    u8 g;
    u8 b;
    u8 a;
} GXColor;

typedef struct _GXColorS10 {
    s16 r;
    s16 g;
    s16 b;
    s16 a;
} GXColorS10;

typedef struct _GXTexObj {
    u32 dummy[8];
} GXTexObj;

typedef struct _GXTexObjPriv {
	u32 mode0;       // _00
	u32 mode1;       // _04
	u32 image0;      // _08
	u32 image3;      // _0C
	void* userData;  // _10
	GXTexFmt format; // _14
	u32 tlutName;    // _18
	u16 loadCount;   // _1C
	u8 loadFormat;   // _1E
	u8 flags;        // _1F
} GXTexObjPriv;

typedef struct _GXLightObj {
    u32 dummy[16];
} GXLightObj;

typedef struct __GXLightObjPriv {
	u32 reserved[3]; // _00
	GXColor color;   // _0C, light color
	f32 a[3];        // _10, angle-attenuation coefficients
	f32 k[3];        // _1C, distance-attenuation coefficients
	f32 lpos[3];     // _28, diffuse: position;  specular: direction
	f32 ldir[3];     // _34, diffuse: direction; specular: half-angle
} GXLightObjPriv;

typedef struct _GXTexRegion {
    u32 dummy[4];
} GXTexRegion;

typedef struct _GXTexRegionPriv {
	u32 unk0;      // _00
	u32 unk4;      // _04
	u32 unk8;      // _08
	u8 unkC;       // _0C
	u8 unkD;       // _0D
	u8 padding[2]; // _0E
} GXTexRegionPriv;

typedef struct _GXTlutObj {
    u32 dummy[3];
} GXTlutObj;

typedef struct _GXTlutObjPriv {
	u32 unk0;        // _00
	u32 unk4;        // _04
	u16 numEntries;  // _08
	u8 padding[0x2]; // _0A
} GXTlutObjPriv;

typedef struct _GXTlutRegion {
    u32 dummy[4];
} GXTlutRegion;

typedef struct _GXTlutRegionPriv {
	u32 unk0;              // _00
	GXTlutObjPriv tlutObj; // _04
} GXTlutRegionPriv;

typedef struct _GXFogAdjTable {
    u16 fogVals[10];
} GXFogAdjTable;

#endif
