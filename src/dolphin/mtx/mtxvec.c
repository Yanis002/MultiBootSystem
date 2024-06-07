#include "dolphin/hw_regs.h"
#include "dolphin/mtx.h"
#include "macros.h"

ASM void PSMTXMultVec(const register Mtx m, const register Vec* src, register Vec* dst){
#ifdef __MWERKS__ // clang-format off
    nofralloc
    psq_l   FP0, 0(src), 0, 0
    psq_l   FP2, 0(m), 0, 0
    psq_l   FP1, 8(src), 1, 0
    ps_mul  FP4, FP2, FP0
    psq_l   FP3, 8(m), 0, 0
    ps_madd FP5, FP3, FP1, FP4
    psq_l   FP8, 16(m), 0, 0
    ps_sum0 FP6, FP5, FP6, FP5
    psq_l   FP9, 24(m), 0, 0
    ps_mul  FP10, FP8, FP0
    psq_st  FP6, 0(dst), 1, 0
    ps_madd FP11, FP9, FP1, FP10
    psq_l   FP2, 32(m), 0, 0
    ps_sum0 FP12, FP11, FP12, FP11
    psq_l   FP3, 40(m), 0, 0
    ps_mul  FP4, FP2, FP0
    psq_st  FP12, 4(dst), 1, 0
    ps_madd FP5, FP3, FP1, FP4
    ps_sum0 FP6, FP5, FP6, FP5
    psq_st  FP6, 8(dst), 1, 0
    blr
#endif // clang-format on
}

ASM void PSMTXMultVecArray(const register Mtx m, const register Vec* srcBase, register Vec* dstBase,
                           register u32 count){
#ifdef __MWERKS__ // clang-format off
    nofralloc
    psq_l       FP13,  0(m), 0, 0
    psq_l       FP12, 16(m), 0, 0
    subi        count, count, 1
    psq_l       FP11,  8(m), 0, 0
    ps_merge00  FP0, FP13, FP12
    subi        dstBase, dstBase, 4
    psq_l       FP10, 24(m), 0, 0
    ps_merge11  FP1, FP13, FP12
    mtctr       count
    psq_l       FP4,  32(m), 0, 0
    ps_merge00  FP2, FP11, FP10
    psq_l       FP5,  40(m), 0, 0
    ps_merge11  FP3, FP11, FP10
    psq_l       FP6,  0(srcBase), 0, 0
    psq_lu      FP7,  8(srcBase), 1, 0
    ps_madds0   FP8, FP0, FP6, FP3
    ps_mul      FP9, FP4, FP6
    ps_madds1   FP8, FP1, FP6, FP8
    ps_madd     FP10, FP5, FP7, FP9

_mloop:
    psq_lu      FP6,  4(srcBase), 0, 0
    ps_madds0   FP12, FP2, FP7, FP8
    psq_lu      FP7,  8(srcBase), 1, 0
    ps_sum0     FP13, FP10, FP9, FP10
    ps_madds0   FP8, FP0, FP6, FP3
    ps_mul      FP9, FP4, FP6
    psq_stu     FP12,  4(dstBase), 0, 0
    ps_madds1   FP8, FP1, FP6, FP8
    psq_stu     FP13,  8(dstBase), 1, 0
    ps_madd     FP10, FP5, FP7, FP9

    bdnz        _mloop
    ps_madds0   FP12, FP2, FP7, FP8
    ps_sum0     FP13, FP10, FP9, FP10
    psq_stu     FP12,  4(dstBase), 0, 0
    psq_stu     FP13,  8(dstBase), 1, 0
    blr
#endif // clang-format on
}

ASM void PSMTXMultVecSR(const register Mtx m, const register Vec* src, register Vec* dst) {
#ifdef __MWERKS__ // clang-format off
    nofralloc
    psq_l   FP0, 0(m), 0, 0
    psq_l   FP6, 0(src), 0, 0
    psq_l   FP2, 16(m), 0, 0
    ps_mul  FP8, FP0, FP6
    psq_l   FP4, 32(m), 0, 0
    ps_mul  FP10, FP2, FP6
    psq_l   FP7, 8(src), 1, 0
    ps_mul  FP12, FP4, FP6
    psq_l   FP3, 24(m), 0, 0
    ps_sum0 FP8, FP8, FP8, FP8
    psq_l   FP5, 40(m), 0, 0
    ps_sum0 FP10, FP10, FP10, FP10
    psq_l   FP1,  8(m), 0, 0
    ps_sum0 FP12, FP12, FP12, FP12
    ps_madd FP9, FP1, FP7, FP8
    psq_st  FP9,  0(dst), 1, 0
    ps_madd FP11, FP3, FP7, FP10
    psq_st  FP11, 4(dst), 1, 0
    ps_madd FP13, FP5, FP7, FP12
    psq_st  FP13, 8(dst), 1, 0
    blr
#endif // clang-format on
}
