#include "dolphin/vec.h"
#include "dolphin/hw_regs.h"
#include "macros.h"

void PSVECNormalize(const register Vec* vec1, register Vec* ret) {
    register f32 half = 0.5f;
    register f32 three = 3.0f;
    register f32 xx_zz, xx_yy;
    register f32 square_sum;
    register f32 ret_sqrt;
    register f32 n_0, n_1;

#ifdef __MWERKS__ // clang-format off
    asm {
        psq_l       FP2, 0(vec1), 0, 0;
        ps_mul      xx_yy, FP2, FP2;
        psq_l       FP3, 8(vec1), 1, 0;
        ps_madd     xx_zz, FP3, FP3, xx_yy;
        ps_sum0     square_sum, xx_zz, FP3, xx_yy;
        frsqrte     ret_sqrt, square_sum;
        fmuls       n_0, ret_sqrt, ret_sqrt;
        fmuls       n_1, ret_sqrt, half;
        fnmsubs     n_0, n_0, square_sum, three;
        fmuls       ret_sqrt, n_0, n_1;
        ps_muls0    FP2, FP2, ret_sqrt;
        psq_st      FP2, 0(ret), 0, 0;
        ps_muls0    FP3, FP3, ret_sqrt;
        psq_st      FP3, 8(ret), 1, 0;
    }
#endif // clang-format on
}

ASM void PSVECCrossProduct(const register Vec* vec1, const register Vec* vec2, register Vec* ret) {
#ifdef __MWERKS__ // clang-format off
    nofralloc
    psq_l       FP1, 0(vec2), 0, 0
    lfs         FP2, 8(vec1)
    psq_l       FP0, 0(vec1), 0, 0
    ps_merge10  FP6, FP1, FP1
    lfs         FP3, 8(vec2)
    ps_mul      FP4, FP1, FP2
    ps_muls0    FP7, FP1, FP0
    ps_msub     FP5, FP0, FP3, FP4
    ps_msub     FP8, FP0, FP6, FP7
    ps_merge11  FP9, FP5, FP5
    ps_merge01  FP10, FP5, FP8
    psq_st      FP9, 0(ret), 1, 0
    ps_neg      FP10, FP10
    psq_st      FP10, 4(ret), 0, 0
    blr
#endif // clang-format on
}
