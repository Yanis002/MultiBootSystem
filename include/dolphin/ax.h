#ifndef _DOLPHIN_AX_H
#define _DOLPHIN_AX_H

#include "dolphin/os/OSTime.h"
#include "dolphin/types.h"

#ifdef __cplusplus
extern "C" {
#endif

#define AX_DSP_SLAVE_LENGTH 3264
#define AX_MAX_VOICES 64

#define AX_SRC_TYPE_NONE 0
#define AX_SRC_TYPE_LINEAR 1
#define AX_SRC_TYPE_4TAP_8K 2
#define AX_SRC_TYPE_4TAP_12K 3
#define AX_SRC_TYPE_4TAP_16K 4

// sync flags
#define AX_SYNC_FLAG_COPYALL (1 << 31)
#define AX_SYNC_FLAG_UNK1 (1 << 30) // reserved, unused?
#define AX_SYNC_FLAG_UNK2 (1 << 29) // reserved, unused?
#define AX_SYNC_FLAG_UNK3 (1 << 28) // reserved, unused?
#define AX_SYNC_FLAG_UNK4 (1 << 27) // reserved, unused?
#define AX_SYNC_FLAG_UNK5 (1 << 26) // reserved, unused?
#define AX_SYNC_FLAG_UNK6 (1 << 25) // reserved, unused?
#define AX_SYNC_FLAG_UNK7 (1 << 24) // reserved, unused?
#define AX_SYNC_FLAG_UNK8 (1 << 23) // reserved, unused?
#define AX_SYNC_FLAG_UNK9 (1 << 22) // reserved, unused?
#define AX_SYNC_FLAG_UNK10 (1 << 21) // reserved, unused?
#define AX_SYNC_FLAG_COPYADPCMLOOP (1 << 20)
#define AX_SYNC_FLAG_COPYRATIO (1 << 19)
#define AX_SYNC_FLAG_COPYSRC (1 << 18)
#define AX_SYNC_FLAG_COPYADPCM (1 << 17)
#define AX_SYNC_FLAG_COPYCURADDR (1 << 16)
#define AX_SYNC_FLAG_COPYENDADDR (1 << 15)
#define AX_SYNC_FLAG_COPYLOOPADDR (1 << 14)
#define AX_SYNC_FLAG_COPYLOOP (1 << 13)
#define AX_SYNC_FLAG_COPYADDR (1 << 12)
#define AX_SYNC_FLAG_COPYFIR (1 << 11)
#define AX_SYNC_FLAG_SWAPVOL (1 << 10)
#define AX_SYNC_FLAG_COPYVOL (1 << 9)
#define AX_SYNC_FLAG_COPYDPOP (1 << 8)
#define AX_SYNC_FLAG_COPYUPDATE (1 << 7)
#define AX_SYNC_FLAG_COPYTSHIFT (1 << 6)
#define AX_SYNC_FLAG_COPYITD (1 << 5)
#define AX_SYNC_FLAG_COPYAXPBMIX (1 << 4)
#define AX_SYNC_FLAG_COPYTYPE (1 << 3)
#define AX_SYNC_FLAG_COPYSTATE (1 << 2)
#define AX_SYNC_FLAG_COPYMXRCTRL (1 << 1)
#define AX_SYNC_FLAG_COPYSELECT (1 << 0)

#define AX_PRIORITY_STACKS 32

typedef void (*AXUserCallback)(void);
typedef void (*AXAuxCallback)(void* data, void* context);
typedef void (*AXVoiceCallback)(void* p);

typedef struct AXProfile {
    /* 0x00 */ OSTime axFrameStart;
    /* 0x08 */ OSTime auxProcessingStart;
    /* 0x10 */ OSTime auxProcessingEnd;
    /* 0x18 */ OSTime userCallbackStart;
    /* 0x20 */ OSTime userCallbackEnd;
    /* 0x28 */ OSTime axFrameEnd;
    /* 0x30 */ u32 axNumVoices;
    /* 0x34 */ u32 __padding;
} AXProfile; // size = 0x38

typedef struct AXPbMix {
    /* 0x00 */ u16 vL;
    /* 0x02 */ u16 vDeltaL;
    /* 0x04 */ u16 vR;
    /* 0x06 */ u16 vDeltaR;
    /* 0x08 */ u16 vAuxAL;
    /* 0x0A */ u16 vDeltaAuxAL;
    /* 0x0C */ u16 vAuxAR;
    /* 0x0E */ u16 vDeltaAuxAR;
    /* 0x10 */ u16 vAuxBL;
    /* 0x12 */ u16 vDeltaAuxBL;
    /* 0x14 */ u16 vAuxBR;
    /* 0x16 */ u16 vDeltaAuxBR;
    /* 0x18 */ u16 vAuxBS;
    /* 0x1A */ u16 vDeltaAuxBS;
    /* 0x1C */ u16 vS;
    /* 0x1E */ u16 vDeltaS;
    /* 0x20 */ u16 vAuxAS;
    /* 0x22 */ u16 vDeltaAuxAS;
} AXPbMix; // size = 0x24

typedef struct AXPBitD {
    /* 0x00 */ u16 flag;
    /* 0x02 */ u16 bufferHi;
    /* 0x04 */ u16 bufferLo;
    /* 0x06 */ u16 shiftL;
    /* 0x08 */ u16 shiftR;
    /* 0x0A */ u16 targetShiftL;
    /* 0x0C */ u16 targetShiftR;
} AXPBitD; // size = 0x0E

typedef struct AXPbUpdate {
    /* 0x00 */ u16 updNum[5];
    /* 0x0A */ u16 dataHi;
    /* 0x0C */ u16 dataLo;
} AXPbUpdate; // size = 0x0E

typedef struct AXPbDPop {
    /* 0x00 */ s16 aL;
    /* 0x02 */ s16 aAuxAL;
    /* 0x04 */ s16 aAuxBL;
    /* 0x06 */ s16 aR;
    /* 0x08 */ s16 aAuxAR;
    /* 0x0A */ s16 aAuxBR;
    /* 0x0C */ s16 aS;
    /* 0x0E */ s16 aAuxAS;
    /* 0x10 */ s16 aAuxBS;
} AXPbDPop; // size = 0x12

typedef struct AXPbVe {
    /* 0x00 */ u16 currentVolume;
    /* 0x02 */ s16 currentDelta;
} AXPbVe; // size = 0x04

typedef struct AXPbFir {
    /* 0x00 */ u16 numCoefs;
    /* 0x02 */ u16 coefsHi;
    /* 0x04 */ u16 coefsLo;
} AXPbFir; // size = 0x06

typedef struct AXPbAddr {
    /* 0x00 */ u16 loopFlag;
    /* 0x02 */ u16 format;
    /* 0x04 */ u16 loopAddressHi;
    /* 0x06 */ u16 loopAddressLo;
    /* 0x08 */ u16 endAddressHi;
    /* 0x0A */ u16 endAddressLo;
    /* 0x0C */ u16 currentAddressHi;
    /* 0x0E */ u16 currentAddressLo;
} AXPbAddr;

typedef struct AXPbADPCM {
    /* 0x00 */ u16 a[8][2];
    /* 0x20 */ u16 gain;
    /* 0x22 */ u16 pred_scale;
    /* 0x24 */ u16 yn1;
    /* 0x26 */ u16 yn2;
} AXPbADPCM; // size = 0x28

typedef struct AXPbSrc {
    /* 0x00 */ u16 ratioHi;
    /* 0x02 */ u16 ratioLo;
    /* 0x04 */ u16 currentAddressFrac;
    /* 0x06 */ u16 last_samples[4];
} AXPbSrc; // size = 0x0E

typedef struct AXPbADPCMLoop {
    /* 0x00 */ u16 loop_pred_scale;
    /* 0x02 */ u16 loop_yn1;
    /* 0x04 */ u16 loop_yn2;
} AXPbADPCMLoop; // size = 0x06

typedef struct AXPbLPF {
    /* 0x00 */ u16 on;
    /* 0x02 */ u16 yn1;
    /* 0x04 */ u16 a0;
    /* 0x06 */ u16 b0;
} AXPbLPF; // size = 0x08

typedef struct AXPb {
    /* 0x00 */ u16 nextHi;
    /* 0x02 */ u16 nextLo;
    /* 0x04 */ u16 currHi;
    /* 0x06 */ u16 currLo;
    /* 0x08 */ u16 srcSelect;
    /* 0x0A */ u16 coefSelect;
    /* 0x0C */ u16 mixerCtrl;
    /* 0x0E */ u16 state;
    /* 0x10 */ u16 type;
    /* 0x12 */ AXPbMix mix;
    /* 0x36 */ AXPBitD itd;
    /* 0x44 */ AXPbUpdate update;
    /* 0x52 */ AXPbDPop dpop;
    /* 0x64 */ AXPbVe ve;
    /* 0x68 */ AXPbFir fir;
    /* 0x6E */ AXPbAddr addr;
    /* 0x7E */ AXPbADPCM adpcm;
    /* 0xA6 */ AXPbSrc src;
    /* 0xB4 */ AXPbADPCMLoop adpcmLoop;
    /* 0xBA */ AXPbLPF lpf;
    /* 0xC2 */ u16 pad[25];
} AXPb; // size = 0xF4

typedef struct AXSPb {
    /* 0x00 */ u16 dpopLHi;
    /* 0x02 */ u16 dpopLLo;
    /* 0x04 */ s16 dpopLDelta;
    /* 0x06 */ u16 dpopRHi;
    /* 0x08 */ u16 dpopRLo;
    /* 0x0A */ s16 dpopRDelta;
    /* 0x0C */ u16 dpopSHi;
    /* 0x0E */ u16 dpopSLo;
    /* 0x10 */ s16 dpopSDelta;
    /* 0x12 */ u16 dpopALHi;
    /* 0x14 */ u16 dpopALLo;
    /* 0x16 */ s16 dpopALDelta;
    /* 0x18 */ u16 dpopARHi;
    /* 0x1A */ u16 dpopARLo;
    /* 0x1C */ s16 dpopARDelta;
    /* 0x1E */ u16 dpopASHi;
    /* 0x20 */ u16 dpopASLo;
    /* 0x22 */ s16 dpopASDelta;
    /* 0x24 */ u16 dpopBLHi;
    /* 0x26 */ u16 dpopBLLo;
    /* 0x28 */ s16 dpopBLDelta;
    /* 0x2A */ u16 dpopBRHi;
    /* 0x2C */ u16 dpopBRLo;
    /* 0x2E */ s16 dpopBRDelta;
    /* 0x20 */ u16 dpopBSHi;
    /* 0x22 */ u16 dpopBSLo;
    /* 0x24 */ s16 dpopBSDelta;
} AXSPb; // size = 0x28

typedef struct AXVPb {
    /* 0x000 */ void* next;
    /* 0x004 */ void* prev;
    /* 0x008 */ void* next1;
    /* 0x00C */ u32 priority;
    /* 0x010 */ AXVoiceCallback callback;
    /* 0x014 */ u32 userContext;
    /* 0x018 */ u32 index;
    /* 0x01C */ u32 sync;
    /* 0x020 */ u32 depop;
    /* 0x024 */ u32 updateMS;
    /* 0x028 */ u32 updateCounter;
    /* 0x02C */ u32 updateTotal;
    /* 0x030 */ u16* updateWrite;
    /* 0x034 */ u16 updateData[128];
    /* 0x134 */ void* itdBuffer;
    /* 0x138 */ AXPb pb;
} AXVPb; // size = 0x22C

typedef struct AXPbu {
    /* 0x00 */ u16 data[128];
} AXPbu; // size = 0x100

typedef struct _AXPBitDBuffer {
    /* 0x00 */ s16 data[32];
} AXPBitDBuffer; // size = 0x40

void AXInit(void);
void AXInitEx(u32 outputBufferMode);
void AXQuit(void);
AXUserCallback AXRegisterCallback(AXUserCallback callback);
void AXSetMode(u32 mode);
u32 AXGetMode(void);
void AXSetMaxDspCycles(u32 cycles);
u32 AXGetMaxDspCycles(void);
u32 AXGetDspCycles(void);
void AXRegisterAuxACallback(AXAuxCallback callback, void* context);
void AXRegisterAuxBCallback(AXAuxCallback callback, void* context);
AXVPb* AXAcquireVoice(u32 priority, AXVoiceCallback callback, u32 userContext);
void AXFreeVoice(AXVPb* p);
void AXSetVoicePriority(AXVPb* p, u32 priority);
void AXSetVoiceSrcType(AXVPb* p, u32 type);
void AXSetVoiceState(AXVPb* p, u16 state);
void AXSetVoiceType(AXVPb* p, u16 type);
void AXSetVoiceMix(AXVPb* p, AXPbMix* mix);
void AXSetVoiceItdOn(AXVPb* p);
void AXSetVoiceItdTarget(AXVPb* p, u16 lShift, u16 rShift);
void AXSetVoiceUpdateIncrement(AXVPb* p);
void AXSetVoiceUpdateWrite(AXVPb* p, u16 param, u16 data);
void AXSetVoiceDpop(AXVPb* p, AXPbDPop* dpop);
void AXSetVoiceVe(AXVPb* p, AXPbVe* ve);
void AXSetVoiceVeDelta(AXVPb* p, s16 delta);
void AXSetVoiceFir(AXVPb* p, AXPbFir* fir);
void AXSetVoiceAddr(AXVPb* p, AXPbAddr* addr);
void AXSetVoiceLoop(AXVPb* p, u16 loop);
void AXSetVoiceLoopAddr(AXVPb* p, u32 address);
void AXSetVoiceEndAddr(AXVPb* p, u32 address);
void AXSetVoiceCurrentAddr(AXVPb* p, u32 address);
void AXSetVoiceAdpcm(AXVPb* p, AXPbADPCM* adpcm);
void AXSetVoiceSrc(AXVPb* p, AXPbSrc* src);
void AXSetVoiceSrcRatio(AXVPb* p, f32 ratio);
void AXSetVoiceAdpcmLoop(AXVPb* p, AXPbADPCMLoop* adpcmloop);
void AXSetVoiceLpf(AXVPb* p, AXPbLPF* lpf);
void AXSetVoiceLpfCoefs(AXVPb* p, u16 a0, u16 b0);
void AXInitProfile(AXProfile* profile, u32 maxProfiles);
u32 AXGetProfile(void);
void AXSetCompressor(u32);
void AXSetStepMode(u32);
void AXGetLpfCoefs(u16 freq, u16* a0, u16* b0);

#ifdef __cplusplus
};
#endif

#endif
