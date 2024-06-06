#include "dolphin/gx.h"
#include "dolphin/hw_regs.h"
#include "dolphin/os.h"
#include "dolphin/vi.h"

u16 OSGetFontEncode(void) {
    static u16 fontEncode = 0xFFFF;
    if (fontEncode <= 1) {
        return fontEncode;
    }
    switch (__OSTVMode) {
        case VI_NTSC:
            fontEncode = (__VIRegs[VI_DTV_STAT] & 2) ? OS_FONT_ENCODE_SJIS : OS_FONT_ENCODE_ANSI;
            break;

        case VI_PAL:
        case VI_MPAL:
        case VI_DEBUG:
        case VI_DEBUG_PAL:
        case VI_EURGB60:
        default:
            fontEncode = OS_FONT_ENCODE_ANSI;
    }

    return fontEncode;
}
