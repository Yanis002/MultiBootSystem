#include "runtime/ptmf.h"
#include "macros.h"

const __ptmf __ptmf_null = { 0, 0, 0 };

ASM void __ptmf_scall(...) {
#ifdef __MWERKS__ // clang-format off
    nofralloc;
    lwz r0, __ptmf.this_delta(r12)
    lwz r11, __ptmf.v_offset(r12)
    lwz r12, __ptmf.f_data(r12)
    add r3, r3, r0
    cmpwi r11, 0x0
    blt do_call
    lwzx r12, r3, r12
    lwzx r12, r12, r11
do_call:
    mtctr r12
    bctr
#endif // clang-format on
};
