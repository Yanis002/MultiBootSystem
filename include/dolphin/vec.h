#ifndef _DOLPHIN_VEC_H
#define _DOLPHIN_VEC_H

#include "dolphin/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct Vec {
    f32 x;
    f32 y;
    f32 z;
} Vec;

typedef struct SVec {
    s16 x;
    s16 y;
    s16 z;
} SVec;

void PSVECAdd(const Vec*, const Vec*, Vec*);
void PSVECSubtract(const Vec*, const Vec*, Vec*);
void PSVECNormalize(const register Vec* vec1, register Vec* ret);
f32 PSVECMag(const Vec*);
void PSVECCrossProduct(const register Vec* vec1, const register Vec* vec2, register Vec* ret);

#ifdef __cplusplus
}
#endif

#endif
