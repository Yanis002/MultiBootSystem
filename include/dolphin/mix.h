#ifndef _DOLPHIN_MIX_H_
#define _DOLPHIN_MIX_H_

#include "dolphin/ax.h"

typedef struct MIXChannel {
    /* 0x00 */ AXVPb* axvpb;
    /* 0x04 */ u32 mode;
    /* 0x08 */ int input;
    /* 0x0C */ int auxA;
    /* 0x10 */ int auxB;
    /* 0x14 */ int pan;
    /* 0x18 */ int span;
    /* 0x1C */ int fader;
    /* 0x20 */ int l;
    /* 0x24 */ int r;
    /* 0x28 */ int f;
    /* 0x2C */ int b;
    /* 0x30 */ u16 v;
    /* 0x32 */ u16 v1;
    /* 0x34 */ u16 vL;
    /* 0x36 */ u16 vL1;
    /* 0x38 */ u16 vR;
    /* 0x3A */ u16 vR1;
    /* 0x3C */ u16 vS;
    /* 0x3E */ u16 vS1;
    /* 0x40 */ u16 vAL;
    /* 0x42 */ u16 vAL1;
    /* 0x44 */ u16 vAR;
    /* 0x46 */ u16 vAR1;
    /* 0x48 */ u16 vAS;
    /* 0x4A */ u16 vAS1;
    /* 0x4C */ u16 vBL;
    /* 0x4E */ u16 vBL1;
    /* 0x50 */ u16 vBR;
    /* 0x52 */ u16 vBR1;
    /* 0x54 */ u16 vBS;
    /* 0x56 */ u16 vBS1;
} MIXChannel;

void MIXInit(void);
void MIXQuit(void);
void MIXInitChannel(AXVPb* axvpb, u32 mode, int input, int auxA, int auxB, int pan, int span, int fader);
void MIXReleaseChannel(AXVPb* axvpb);
void MIXResetControls(AXVPb* p);
void MIXSetInput(AXVPb* p, int dB);
void MIXAdjustInput(AXVPb* p, int dB);
int MIXGetInput(AXVPb* p);
void MIXAuxAPostFader(AXVPb* p);
void MIXAuxAPreFader(AXVPb* p);
int MIXAuxAIsPostFader(AXVPb* p);
void MIXSetAuxA(AXVPb* p, int dB);
void MIXAdjustAuxA(AXVPb* p, int dB);
int MIXGetAuxA(AXVPb* p);
void MIXAuxBPostFader(AXVPb* p);
void MIXAuxBPreFader(AXVPb* p);
int MIXAuxBIsPostFader(AXVPb* p);
void MIXSetAuxB(AXVPb* p, int dB);
void MIXAdjustAuxB(AXVPb* p, int dB);
int MIXGetAuxB(AXVPb* p);
void MIXSetPan(AXVPb* p, int pan);
void MIXAdjustPan(AXVPb* p, int pan);
int MIXGetPan(AXVPb* p);
void MIXSetSPan(AXVPb* p, int span);
void MIXAdjustSPan(AXVPb* p, int span);
int MIXGetSPan(AXVPb* p);
void MIXMute(AXVPb* p);
void MIXUnMute(AXVPb* p);
int MIXIsMute(AXVPb* p);
void MIXSetFader(AXVPb* p, int dB);
void MIXAdjustFader(AXVPb* p, int dB);
int MIXGetFader(AXVPb* p);
void MIXSetDvdStreamFader(int dB);
int MIXGetDvdStreamFader(void);
void MIXUpdateSettings(void);

#endif // _DOLPHIN_MIX_H_
