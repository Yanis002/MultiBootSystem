#include "__ppc_eabi_linker.h"
#include "macros.h"
#include "runtime/global_destructor_chain.h"

#if __MWERKS__
#pragma exceptions off
#pragma internal on
#endif

#ifdef __cplusplus
extern "C" {
#endif

extern void __init_cpp_exceptions(void);
extern void __fini_cpp_exceptions(void);

#ifdef __cplusplus
}
#endif

static int fragmentID = -2;

static ASM char* GetR2() {
#ifdef __MWERKS__ // clang-format off
    nofralloc;
    mr r3, r2
    blr
#endif // clang-format on
}

void __init_cpp_exceptions(void) {
    char* R2;
    if (fragmentID == -2) {
        R2 = GetR2();
        fragmentID = __register_fragment(_eti_init_info, R2);
    }
}

void __fini_cpp_exceptions(void) {
    if (fragmentID != -2) {
        __unregister_fragment(fragmentID);
        fragmentID = -2;
    }
}

CTORS static void* const __init_cpp_exceptions_reference = __init_cpp_exceptions;
DTORS static void* const __destroy_global_chain_reference = __destroy_global_chain;
DTORS static void* const __fini_cpp_exceptions_reference = __fini_cpp_exceptions;
