#include "dolphin.h"

u32 mt[0x270];
s32 mti = 0x271;

u32 sgenrand(u32 arg0) {
    int i;

    for (i = 0; i < 0x270; i++) {
        mt[i] = arg0 & ~0xFFFF;
        arg0 = arg0 * 0x10DCD + 1;
        mt[i] |= (arg0 >> 0x10);
        arg0 = arg0 * 0x10DCD + 1;
    }

    mti = 0x270;
    return arg0;
}
