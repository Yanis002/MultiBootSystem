#include "dolphin.h"
#include "JSystem/JKernel/JKRAram.h"

typedef struct SEStatus {
    AXVPb* axvpb;
    SPSoundEntry* pSoundEntry;
} SEStatus;

u32 aramMemArray[3];
SEStatus se_voice[64];
u8 xfer_buffer[0x4000];

static u32 aramZeroBase;
static u32 aramUserBase;
static SPSoundTable* sp_table;
static s32 okaiVolume;
static s32 selectVolume;
static s32 cancelVolume;
static s32 startVolume;

void ax_se_callback() {
    s32 iStatus;

    for (iStatus = 0; iStatus < ARRAY_COUNT(se_voice); iStatus++) {
        if (se_voice[iStatus].axvpb != NULL && se_voice[iStatus].axvpb->pb.state == 0) {
            MIXReleaseChannel(se_voice[iStatus].axvpb);
            AXFreeVoice(se_voice[iStatus].axvpb);
            se_voice[iStatus].axvpb = NULL;
        }
    }
}

// non-matching
void seInit(SEStatus*) {
    s32 i;

    DVDInit();
    ARInit(aramMemArray, 3);
    ARQInit();
    AIInit(NULL);
    AXInit();
    MIXInit();

    JKRAllocFromAram(0x800000, JKRAramHeap::HEAD);
    // aramUserBase = alloc__11JKRAramHeapFUlQ211JKRAramHeap10EAllocMode(sAramObject__7JKRAram.unk0->unk78, 0x800000U, (JKRAramHeap::EAllocMode) 0);

    AMInit(aramUserBase, 0x800000);
    aramZeroBase = AMGetZeroBuffer();
    sp_table = (SPSoundTable*)AMLoadFile("multiple_se.spt", NULL);
    aramUserBase = __AMPushBuffered("multiple_se.spd", xfer_buffer, sizeof(xfer_buffer), NULL, false);
    SPInitSoundTable(sp_table, aramUserBase, aramZeroBase);

    for (i = 0; i < 64 /* ARRAY_COUNT(se_voice) */; i++) {
        se_voice[i].axvpb = NULL;
        se_voice[i].pSoundEntry = NULL;
    }

    AXRegisterCallback(ax_se_callback);
    // okaiVolume = status->okaiVolume;
    // selectVolume = status->selectVolume;
    // cancelVolume = status->cancelVolume;
    // startVolume = status->startVolume;
}

static SEStatus* seGetStatus() {
    s32 iStatus;

    for (iStatus = 0; iStatus < ARRAY_COUNT(se_voice); iStatus++) {
        if (se_voice[iStatus].axvpb == NULL) {
            return &se_voice[iStatus];
        }
    }

    return NULL;
}

void sePlaySE(u32 index) {
    SEStatus* pSEVoice;
    BOOL interrupts;

    interrupts = OSDisableInterrupts();
    pSEVoice = seGetStatus();

    if (pSEVoice != NULL) {
        pSEVoice->axvpb = AXAcquireVoice(0xF, NULL, 0);

        if (pSEVoice->axvpb != NULL) {
            pSEVoice->pSoundEntry = SPGetSoundEntry(sp_table, index);
            SPPrepareSound(pSEVoice->pSoundEntry, pSEVoice->axvpb, pSEVoice->pSoundEntry->sampleRate);

            switch (index) {
                case 0:
                    MIXInitChannel(pSEVoice->axvpb, 0, cancelVolume, -0x3C0, -0x3C0, 0x40, 0x7F, 0);
                    break;
                case 1:
                    MIXInitChannel(pSEVoice->axvpb, 0, okaiVolume, -0x3C0, -0x3C0, 0x40, 0x7F, 0);
                    break;
                case 2:
                    MIXInitChannel(pSEVoice->axvpb, 0, selectVolume, -0x3C0, -0x3C0, 0x40, 0x7F, 0);
                    break;
                case 3:
                    MIXInitChannel(pSEVoice->axvpb, 0, startVolume, -0x3C0, -0x3C0, 0x40, 0x7F, 0);
                    break;
                default:
                    MIXInitChannel(pSEVoice->axvpb, 0, 0, -0x3C0, -0x3C0, 0x40, 0x7F, 0);
                    break;
            }

            AXSetVoiceState(pSEVoice->axvpb, 1);
            OSRestoreInterrupts(interrupts);
            return;
        }

        OSRestoreInterrupts(interrupts);
        OSReport("SFX: AX Voice allocation failed.\n");
        return;
    }

    OSRestoreInterrupts(interrupts);
    OSReport("(No free voices in abstraction layer)\n");
}

void seQuit(void) {
    s32 i;
    BOOL interrupts;

    interrupts = OSDisableInterrupts();

    for (i = 0; i < ARRAY_COUNT(se_voice); i++) {
        if (se_voice[i].axvpb != NULL) {
            AXSetVoiceState(se_voice[i].axvpb, 0);
        }
    }

    OSRestoreInterrupts(interrupts);
}
