#ifndef _MENU_SOUNDEFFECT_HPP
#define _MENU_SOUNDEFFECT_HPP

#define NUM_VOICE 64

typedef struct SEVoice {
    /* 0x00 */ AXVPb* axvpb;
    /* 0x04 */ SPSoundEntry* pSoundEntry;
} SEVoice; // size = 0x08

typedef struct SEStatus {
    /* 0x00 */ u8 volumeLeft;
    /* 0x01 */ u8 volumeRight;
    /* 0x02 */ char pad0[2];
    /* 0x04 */ s32 okaiVolume;
    /* 0x08 */ s32 cancelVolume;
    /* 0x0C */ s32 selectVolume;
    /* 0x10 */ s32 startVolume;
} SEStatus; // size = 0x14

extern void ax_se_callback(void);
extern void seInit(SEStatus* status);
extern void sePlaySE(u32 index);
extern void seQuit(void);

#endif
