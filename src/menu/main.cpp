#include "dolphin.h"
#include "JSystem/JUtility/JUTVideo.h"
#include "JSystem/JUtility/JUTException.h"
#include "new.hpp"
#include "menu/soundEffect.hpp"

static ExecUnk tgc_4840;

static s32 vibrationMode;
static s32 progressiveMode;
static s32 s_errorState;
static SEStatus s_seStatus;

extern void resetSystem(void);
extern void musicstopCallback(void);
extern void waitDTKStop(void);
extern void Init_DTK_System(void);

extern char* UNK_80003100 AT_ADDRESS(OS_BASE_CACHED | 0x3100);
extern u8 UNK_80003120 AT_ADDRESS(OS_BASE_CACHED | 0x3120);
extern u8 UNK_80003121 AT_ADDRESS(OS_BASE_CACHED | 0x3121);

void resetSystem(void) {
    JUTGamePad gamepad(JUTGamePad::Port_1);
    s32 sp8;

    JUTVideo::destroyManager();
    GXSetDrawDoneCallback(NULL);
    VISetBlack(true);
    VIFlush();
    VIWaitForRetrace();

    while ((gamepad.getButton() & 0x1600) == 0x1600) {
        gamepad.read();
    }

    if (sp8 != 0) {
        sp8 = gamepad.mButtonReset.sResetOccurredPort;
    }

    if (gamepad.mButtonReset.sResetOccurredPort != 0) {
        if (sp8 < 0) {
            gamepad.clearForReset();
        } else {
            gamepad.mSuppressPadReset = 0xF0000000;
            gamepad.mRumble.setEnabled(0);
            gamepad.clearForReset();
        }
    } else {
        gamepad.clearForReset();
    }

    DTKFlushTracks(musicstopCallback);
    seQuit();
    OSDisableScheduler();
    OSSetResetCallback(NULL);
    PADSetSamplingCallback(NULL);
    GXSetTexRegionCallback(NULL);
    GXSetTlutRegionCallback(NULL);
    GXSetBreakPtCallback(NULL);
    GXSetDrawSyncCallback(NULL);
    GXSetDrawDoneCallback(NULL);
    AIRegisterDMACallback(NULL);
    AIRegisterStreamCallback(NULL);
    VISetPreRetraceCallback(NULL);
    VISetPostRetraceCallback(NULL);
    __PADDisableRecalibration(false);

    if (s_errorState & 0x08) {
        OSResetSystem(1, 0x80000000, false);
    } else {
        OSResetSystem(0, 0x80000000, false);
    }
}

void myExceptionCallback(u16 arg0, OSContext* context, u32 arg2, u32 arg3) {
    JUTException::waitTime(3000);
}

void fault_callback_scroll(u16 arg0, OSContext* arg1, u32 arg2, u32 arg3) {

}

void initSound(void) {
    DVDCommandBlock block;

    if (DVDGetDriveStatus() != -1) {
        DVDCancelStream(&block);

        if (DVDGetDriveStatus() != -1) {
            Init_DTK_System();

            if (DVDGetDriveStatus() != -1) {
                DTKSetVolume(s_seStatus.volumeLeft, s_seStatus.volumeRight);

                if (DVDGetDriveStatus() != -1) {
                    seInit(&s_seStatus);
                }
            }
        }
    }
}

void exec(int arg0, int arg1) {
    // JUTVideo* var_r3;
    s32 var_r3;
    s32 temp_r0;
    s32 temp_r0_2;
    s32 temp_r3;
    s32 temp_r4;
    s32 temp_r5;
    s32 temp_r5_2;
    s32 var_ctr_2;
    s32 var_r6;
    char* temp_r30;
    u32 var_ctr;

    static char* tgcPaths[14] = {
        "zlj_f.tgc",
        "zlj_f.tgc",
        "S_MarioParty4.tgc",
        "S_Biohazard0.tgc",
        "S_KinnikuManII.tgc",
        "S_MrDori.tgc",
        "S_FZero.tgc",
        "S_Biohazard4.tgc",
        "S_DeadPhoenix.tgc",
        "S_VJ.tgc",
        "S_PN03.tgc",
        "S_Killer7.tgc",
        "S_ZeldaADV.tgc",
        "S_FinalFantasyADV.tgc",
    };

    static char* romNames[14] = {
        "zlj_f.n64",
        "urazlj_f.n64",
        "movie1",
        "movie2",
        "movie3",
        "movie4",
        "movie5",
        "movie6",
        "movie7",
        "movie8",
        "movie9",
        "movie10",
        "movie11",
        "movie12",
    };

    static char* argv[10];
    static s32 argc;
    static u8 init;

    temp_r5 = arg0 * 4;
    temp_r30 = tgcPaths[arg0];

    if (init == 0) {
        argc = 0;
        init = 1;
    }

    if (arg1 == 0) {
        seQuit();

        if (DVDConvertPathToEntrynum(temp_r30) != -1) {
            if (TGCOpen(temp_r30, &tgc_4840) != 0) {
                resetSystem();
                OSResetSystem(1, 1, false);
            }
        } else {
            resetSystem();
            OSResetSystem(1, 1, false);
        }

        return;
    }

    argv[argc++] = romNames[arg0];

    if (vibrationMode != 0) {
        argv[argc++] = "-V1";
    } else {
        argv[argc++] = "-V0";
    }

    if (progressiveMode != 0) {
        argv[argc++] = "-P1";
    } else {
        argv[argc++] = "-P0";
    }

    var_r6 = 0;
    argv[argc++] = "-C1";
    var_r3 = 0;
    argv[argc++] = NULL;

    if (argc > 0) {
        var_r3 = argc - 8;

        if (argc > 8) {
            var_ctr = (u32) (var_r3 + 7) >> 3U;
            if (var_r3 > 0) {
                do {
                    var_r6 += 8;
                    var_ctr -= 1;
                } while (var_ctr != 0);
            }
        }

        var_ctr_2 = argc - var_r6;
        if (var_r6 < argc) {
            do {
                var_ctr_2--;
            } while (var_ctr_2 != 0);
        }
    }

    GXSetDrawDoneCallback(NULL);
    VISetBlack(true);
    VIFlush();
    VIWaitForRetrace();

    if (arg0 > 1) {
        strcpy(UNK_80003100, "OCARINA DISC");
        UNK_80003120 = 1;
        UNK_80003121 = arg0;
    }

    DTKFlushTracks(musicstopCallback);
    waitDTKStop();
    TGCExec(&tgc_4840, 0, argv);
}

void dtkCallback(u32) {

}

void musicstopCallback(void) {

}
