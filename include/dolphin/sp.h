#ifndef _DOLPHIN_SP_H
#define _DOLPHIN_SP_H

#include "dolphin/ax.h"
#include "dolphin/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    /* 0x00 */ AXPbADPCM adpcm;
    /* 0x0A */ AXPbADPCMLoop adpcmloop;
} SPAdpcmEntry; // size = 0x10

typedef struct {
    /* 0x00 */ u32 type;
    /* 0x04 */ u32 sampleRate;
    /* 0x08 */ u32 loopAddr;
    /* 0x0C */ u32 loopEndAddr;
    /* 0x10 */ u32 endAddr;
    /* 0x14 */ u32 currentAddr;
    /* 0x18 */ SPAdpcmEntry* adpcm;
} SPSoundEntry; // size = 0x28

typedef struct {
    /* 0x00 */ u32 entries;
    /* 0x04 */ SPSoundEntry sound;
} SPSoundTable; // size = 0x2C

void SPInitSoundTable(SPSoundTable* table, u32 aramBase, u32 zeroBase);
SPSoundEntry* SPGetSoundEntry(SPSoundTable* table, u32 index);
void SPPrepareSound(SPSoundEntry* sound, AXVPb* axvpb, u32 sampleRate);
void SPPrepareEnd(SPSoundEntry* sound, AXVPb* axvpb);

#ifdef __cplusplus
}
#endif

#endif
