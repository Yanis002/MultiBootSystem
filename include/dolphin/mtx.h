#ifndef _DOLPHIN_MTX_H_
#define _DOLPHIN_MTX_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "dolphin/types.h"
#include "dolphin/vec.h"

#define MTXDegToRad(a) ((a) * 0.01745329252f)

typedef struct Quaternion {
    f32 x, y, z, w;
} Quaternion;

typedef f32 Mtx[3][4];
typedef f32 Mtx23[2][3];
typedef f32 Mtx33[3][3];
typedef f32 Mtx44[4][4];
typedef f32 (*Mtx33Ptr)[3];
typedef f32 (*Mtx44Ptr)[4];
typedef const f32 (*CMtx44Ptr)[4]; // Change name later?

void PSMTXIdentity(Mtx m);
void PSMTXConcat(const Mtx a, const Mtx b, Mtx ab);
void PSMTXTrans(Mtx m, f32 xT, f32 yT, f32 zT);
void PSMTXTransApply(const Mtx src, Mtx dst, f32 xT, f32 yT, f32 zT);
void PSMTXScale(Mtx m, f32 xS, f32 yS, f32 zS);
void PSMTXScaleApply(const Mtx src, Mtx dst, f32 xS, f32 yS, f32 zS);

void PSMTXMultVec(const Mtx m, const Vec* src, Vec* dst);

void C_MTXPerspective(Mtx44 m, f32 fovY, f32 aspect, f32 n, f32 f);
void C_MTXOrtho(Mtx44 m, f32 t, f32 b, f32 l, f32 r, f32 n, f32 f);
void PSMTX44Concat(const Mtx44 a, const Mtx44 b, Mtx44 ab);

void PSMTXCopy(const register Mtx src, register Mtx dst);
u32 PSMTXInverse(const Mtx src, Mtx inv);
void PSMTXRotRad(Mtx m, u8 axis, f32 rad);
void PSMTXRotTrig(register Mtx m, register u8 axis, register f32 sinA, register f32 cosA);
void PSMTXRotAxisRad(Mtx m, const Vec* axis, f32 rad);
void PSMTXQuat(Mtx m, const Quaternion* q);

void C_MTXLightPerspective(Mtx m, f32 fovY, f32 aspect, f32 scaleS, f32 scaleT, f32 transS, f32 transT);
void PSMTXMultVecArray(const register Mtx m, const register Vec* srcBase, register Vec* dstBase, register u32 count);
void PSMTXMultVecSR(const register Mtx m, const register Vec* src, register Vec* dst);

#define MTXIdentity PSMTXIdentity
#define MTXCopy PSMTXCopy
#define MTXConcat PSMTXConcat
#define MTXInverse PSMTXInverse
#define MTXRotRad PSMTXRotRad
#define MTXRotTrig PSMTXRotTrig
#define MTXRotAxisRad PSMTXRotAxisRad
#define MTXTrans PSMTXTrans
#define MTXTransApply PSMTXTransApply
#define MTXScale PSMTXScale
#define MTXScaleApply PSMTXScaleApply
#define MTXQuat PSMTXQuat

#ifdef __cplusplus
};
#endif

#endif
