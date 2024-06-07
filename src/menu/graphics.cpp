#include "JSystem/JUtility/JUTXfb.h"
#include "dolphin/gx.h"

void initGraphics(void) {
    GXColor clearColor = {0};

    GXSetZMode(GX_TRUE, GX_ALWAYS, GX_TRUE);
    GXSetCopyClear(clearColor, 0xFFFFFF);
    GXCopyDisp(JUTXfb::getManager()->getBuffer(0), GX_TRUE);
}
