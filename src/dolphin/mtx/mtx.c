#include "dolphin/mtx.h"
#include "dolphin/hw_regs.h"
#include "macros.h"
#include "math.h"

#define qr0 0

static f32 Unit01[] = {0.0f, 1.0f};

void PSMTXIdentity(register Mtx m) {
    register f32 zero_c;
    register f32 one_c;
    register f32 c_01;
    register f32 c_10;

    // fixes float ordering issue
    (void)1.0f;

    zero_c = 0.0f;
    one_c = 1.0f;

#ifdef __MWERKS__ // clang-format off
    asm {
        psq_st zero_c, 8(m), 0, 0
        ps_merge01 c_01, zero_c, one_c
        psq_st zero_c, 24(m), 0, 0
        ps_merge10 c_10, one_c, zero_c
        psq_st zero_c, 32(m), 0, 0
        psq_st c_01, 16(m), 0, 0
        psq_st c_10, 0(m), 0, 0
        psq_st c_10, 40(m), 0, 0
    }
#endif // clang-format on
}

ASM void PSMTXCopy(const register Mtx src, register Mtx dst){
#ifdef __MWERKS__ // clang-format off
    nofralloc
    psq_l   FP0, 0(src), 0, qr0
    psq_st  FP0, 0(dst), 0, qr0
    psq_l   FP1, 8(src), 0, qr0
    psq_st  FP1, 8(dst), 0, qr0
    psq_l   FP2, 16(src), 0, qr0
    psq_st  FP2, 16(dst), 0, qr0
    psq_l   FP3, 24(src), 0, qr0
    psq_st  FP3, 24(dst), 0, qr0
    psq_l   FP4, 32(src), 0, qr0
    psq_st  FP4, 32(dst), 0, qr0
    psq_l   FP5, 40(src), 0, qr0
    psq_st  FP5, 40(dst), 0, qr0
    blr
#endif // clang-format on
}

ASM void PSMTXConcat(const register Mtx mA, const register Mtx mB, register Mtx mAB) {
#ifdef __MWERKS__ // clang-format off
    nofralloc
    stwu    r1, -64(r1);
    psq_l   FP0, 0(mA), 0, 0;
    stfd    fp14, 8(r1);
    psq_l   FP6, 0(mB), 0, 0;
    addis   r6, 0, Unit01@ha;
    psq_l   FP7, 8(mB), 0, 0;
    stfd    fp15, 16(r1)
    addi    r6, r6, Unit01@l;
    stfd    fp31, 40(r1)
    psq_l   FP8, 16(mB), 0, 0
    ps_muls0 FP12, FP6, FP0
    psq_l   FP2, 16(mA), 0, 0
    ps_muls0 FP13, FP7, FP0
    psq_l   FP31, 0(r6), 0, 0
    ps_muls0 FP14, FP6, FP2
    psq_l   FP9, 24(mB), 0, 0
    ps_muls0 FP15, FP7, FP2
    psq_l   FP1, 8(mA), 0, 0
    ps_madds1 FP12, FP8, FP0, FP12
    psq_l   FP3, 24(mA), 0, 0
    ps_madds1 FP14, FP8, FP2, FP14
    psq_l   FP10, 32(mB), 0, 0
    ps_madds1 FP13, FP9, FP0, FP13
    psq_l   FP11, 40(mB), 0, 0
    ps_madds1 FP15, FP9, FP2, FP15
    psq_l   FP4, 32(mA), 0, 0
    psq_l   FP5, 40(mA), 0, 0
    ps_madds0 FP12, FP10, FP1, FP12
    ps_madds0 FP13, FP11, FP1, FP13
    ps_madds0 FP14, FP10, FP3, FP14
    ps_madds0 FP15, FP11, FP3, FP15
    psq_st  FP12, 0(mAB), 0, 0

    ps_muls0 FP2, FP6, FP4
    ps_madds1 FP13, FP31, FP1, FP13
    ps_muls0 FP0, FP7, FP4
    psq_st  FP14, 16(mAB), 0, 0
    ps_madds1 FP15, FP31, FP3, FP15

    psq_st  FP13, 8(mAB), 0, 0

    ps_madds1 FP2, FP8, FP4, FP2
    ps_madds1 FP0, FP9, FP4, FP0
    ps_madds0 FP2, FP10, FP5, FP2
    lfd    fp14, 8(r1)
    psq_st  FP15, 24(mAB), 0, 0
    ps_madds0 FP0, FP11, FP5, FP0
    psq_st  FP2, 32(mAB), 0, 0
    ps_madds1 FP0, FP31, FP5, FP0
    lfd    fp15, 16(r1)
    psq_st  FP0, 40(mAB), 0, 0

    lfd    fp31, 40(r1)
    addi   r1, r1, 64

    blr
#endif // clang-format on
}

void PSMTXRotRad(Mtx m, u8 axis, f32 rad) {
    f32 sinA, cosA;

    sinA = sinf(rad);
    cosA = cosf(rad);

    PSMTXRotTrig(m, axis, sinA, cosA);
}

void PSMTXRotTrig(register Mtx m, register u8 axis, register f32 sinA, register f32 cosA) {
    register f32 fc0, fc1, nsinA;
    register f32 fw0, fw1, fw2, fw3;

#ifdef __MWERKS__ // clang-format off
    asm {
        frsp    sinA, sinA
        frsp    cosA, cosA
    }
#endif // clang-format on

    fc0 = 0.0f;
    fc1 = 1.0f;

#ifdef __MWERKS__ // clang-format off
    asm {
        ori     axis, axis, 0x20
        ps_neg  nsinA, sinA
        cmplwi  axis, 'x'
        beq     _case_x
        cmplwi  axis, 'y'
        beq     _case_y
        cmplwi  axis, 'z'
        beq     _case_z
        b       _end

_case_x:
        psq_st      fc1,  0(m), 1, 0
        psq_st      fc0,  4(m), 0, 0
        ps_merge00  fw0, sinA, cosA
        psq_st      fc0, 12(m), 0, 0
        ps_merge00  fw1, cosA, nsinA
        psq_st      fc0, 28(m), 0, 0
        psq_st      fc0, 44(m), 1, 0
        psq_st      fw0, 36(m), 0, 0
        psq_st      fw1, 20(m), 0, 0
        b           _end;

_case_y:
        ps_merge00  fw0, cosA, fc0
        ps_merge00  fw1, fc0, fc1
        psq_st      fc0, 24(m), 0, 0
        psq_st      fw0,  0(m), 0, 0
        ps_merge00  fw2, nsinA, fc0
        ps_merge00  fw3, sinA, fc0
        psq_st      fw0, 40(m), 0, 0;
        psq_st      fw1, 16(m), 0, 0;
        psq_st      fw3,  8(m), 0, 0;
        psq_st      fw2, 32(m), 0, 0;
        b           _end;

_case_z:
        psq_st      fc0,  8(m), 0, 0
        ps_merge00  fw0, sinA, cosA
        ps_merge00  fw2, cosA, nsinA
        psq_st      fc0, 24(m), 0, 0
        psq_st      fc0, 32(m), 0, 0
        ps_merge00  fw1, fc1, fc0
        psq_st      fw0, 16(m), 0, 0
        psq_st      fw2,  0(m), 0, 0
        psq_st      fw1, 40(m), 0, 0

_end:
    }
#endif // clang-format on
}

static void __PSMTXRotAxisRadInternal(register Mtx m, const register Vec* axis, register f32 sT, register f32 cT) {
    register f32 tT, fc0;
    register f32 tmp0, tmp1, tmp2, tmp3, tmp4;
    register f32 tmp5, tmp6, tmp7, tmp8, tmp9;

    tmp9 = 0.5f;
    tmp8 = 3.0f;

#ifdef __MWERKS__ // clang-format off
    asm {
        frsp        cT, cT
        psq_l       tmp0, 0(axis), 0, 0
        frsp        sT, sT
        lfs         tmp1, 8(axis)
        ps_mul      tmp2, tmp0, tmp0
        fadds       tmp7, tmp9, tmp9
        ps_madd     tmp3, tmp1, tmp1, tmp2
        fsubs       fc0, tmp9, tmp9
        ps_sum0     tmp4, tmp3, tmp1, tmp2
        fsubs       tT, tmp7, cT
        frsqrte     tmp5, tmp4
        fmuls       tmp2, tmp5, tmp5
        fmuls       tmp3, tmp5, tmp9
        fnmsubs     tmp2, tmp2, tmp4, tmp8
        fmuls       tmp5, tmp2, tmp3
        ps_merge00  cT, cT, cT
        ps_muls0    tmp0, tmp0, tmp5
        ps_muls0    tmp1, tmp1, tmp5
        ps_muls0    tmp4, tmp0, tT
        ps_muls0    tmp9, tmp0, sT
        ps_muls0    tmp5, tmp1, tT
        ps_muls1    tmp3, tmp4, tmp0
        ps_muls0    tmp2, tmp4, tmp0
        ps_muls0    tmp4, tmp4, tmp1
        fnmsubs     tmp6, tmp1, sT, tmp3
        fmadds      tmp7, tmp1, sT, tmp3
        ps_neg      tmp0, tmp9
        ps_sum0     tmp8, tmp4, fc0, tmp9
        ps_sum0     tmp2, tmp2, tmp6, cT
        ps_sum1     tmp3, cT, tmp7, tmp3
        ps_sum0     tmp6, tmp0, fc0 ,tmp4
        psq_st      tmp8, 8(m), 0, 0
        ps_sum0     tmp0, tmp4, tmp4, tmp0
        psq_st      tmp2, 0(m), 0, 0
        ps_muls0    tmp5, tmp5, tmp1
        psq_st      tmp3, 16(m), 0, 0
        ps_sum1     tmp4, tmp9, tmp0, tmp4
        psq_st      tmp6, 24(m), 0, 0
        ps_sum0     tmp5, tmp5, fc0, cT
        psq_st      tmp4, 32(m), 0, 0
        psq_st      tmp5, 40(m), 0, 0
    }
#endif // clang-format on
}

void PSMTXRotAxisRad(Mtx m, const Vec* axis, f32 rad) {
    f32 sinT, cosT;

    sinT = sinf(rad);
    cosT = cosf(rad);

    __PSMTXRotAxisRadInternal(m, axis, sinT, cosT);
}

void PSMTXTrans(register Mtx m, register f32 xT, register f32 yT, register f32 zT) {
    register f32 c0 = 0.0F;
    register f32 c1 = 1.0F;

#ifdef __MWERKS__ // clang-format off
    asm {
        stfs        xT,     12(m)
        stfs        yT,     28(m)
        psq_st      c0,      4(m), 0, 0
        psq_st      c0,     32(m), 0, 0
        stfs        c0,     16(m)
        stfs        c1,     20(m)
        stfs        c0,     24(m)
        stfs        c1,     40(m)
        stfs        zT,     44(m)
        stfs        c1,      0(m)
    }
#endif // clang-format on
}

ASM void PSMTXTransApply(const register Mtx src, register Mtx dst, register f32 xT, register f32 yT, register f32 zT) {
#ifdef __MWERKS__ // clang-format off
    nofralloc;
    psq_l       fp4, 0(src),        0, 0
    frsp        xT, xT
    psq_l       fp5, 8(src),        0, 0
    frsp        yT, yT
    psq_l       fp7, 24(src),       0, 0
    frsp        zT, zT
    psq_l       fp8, 40(src),       0, 0
    psq_st      fp4, 0(dst),        0, 0
    ps_sum1     fp5, xT, fp5, fp5
    psq_l       fp6, 16(src),       0, 0   
    psq_st      fp5, 8(dst),        0, 0
    ps_sum1     fp7, yT, fp7, fp7
    psq_l       fp9, 32(src),       0, 0
    psq_st      fp6, 16(dst),       0, 0
    ps_sum1     fp8, zT, fp8, fp8
    psq_st      fp7, 24(dst),       0, 0
    psq_st      fp9, 32(dst),       0, 0
    psq_st      fp8, 40(dst),       0, 0
    blr
#endif // clang-format on
}

void PSMTXScale(register Mtx m, register f32 xS, register f32 yS, register f32 zS) {
    register f32 c0 = 0.0F;

#ifdef __MWERKS__ // clang-format off
    asm {
        stfs        xS,      0(m)
        psq_st      c0,      4(m), 0, 0
        psq_st      c0,     12(m), 0, 0
        stfs        yS,     20(m)
        psq_st      c0,     24(m), 0, 0
        psq_st      c0,     32(m), 0, 0
        stfs        zS,     40(m)
        stfs        c0,     44(m)
    }
#endif // clang-format on
}

ASM void PSMTXScaleApply(const register Mtx src, register Mtx dst, register f32 xS, register f32 yS, register f32 zS) {
#ifdef __MWERKS__ // clang-format off
    nofralloc
    frsp        xS, xS
    psq_l       fp4, 0(src),  0, 0
    frsp        yS, yS
    psq_l       fp5, 8(src),  0, 0
    frsp        zS, zS
    ps_muls0    fp4, fp4, xS
    psq_l       fp6, 16(src), 0, 0
    ps_muls0    fp5, fp5, xS
    psq_l       fp7, 24(src), 0, 0
    ps_muls0    fp6, fp6, yS
    psq_l       fp8, 32(src), 0, 0
    psq_st      fp4, 0(dst),  0, 0
    ps_muls0    fp7, fp7, yS
    psq_l       fp2, 40(src), 0, 0
    psq_st      fp5, 8(dst),  0, 0
    ps_muls0    fp8, fp8, zS
    psq_st      fp6, 16(dst), 0, 0
    ps_muls0    fp2, fp2, zS
    psq_st      fp7, 24(dst), 0, 0
    psq_st      fp8, 32(dst), 0, 0
    psq_st      fp2, 40(dst), 0, 0
    blr
#endif // clang-format on
}

void C_MTXLightPerspective(Mtx m, f32 fovY, f32 aspect, f32 scaleS, f32 scaleT, f32 transS, f32 transT) {
    f32 angle;
    f32 cot;

    // hacky .sdata2 ordering fix
    (void)-1.0f;

    angle = fovY * 0.5f;
    angle = MTXDegToRad(angle);

    cot = 1.0f / tanf(angle);

    m[0][0] = (cot / aspect) * scaleS;
    m[0][1] = 0.0f;
    m[0][2] = -transS;
    m[0][3] = 0.0f;

    m[1][0] = 0.0f;
    m[1][1] = cot * scaleT;
    m[1][2] = -transT;
    m[1][3] = 0.0f;

    m[2][0] = 0.0f;
    m[2][1] = 0.0f;
    m[2][2] = -1.0f;
    m[2][3] = 0.0f;
}
