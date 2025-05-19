#include "dolphin.h"
#include "macros.h"

TGCHeader THeader ATTRIBUTE_ALIGN(32);

static volatile BOOL Prepared;
static s32 ReadDone;
static OSThreadQueue __TGCThreadQueue;

void PrepareExecCallback(s32 result, DVDCommandBlock* block);
void cbForTGCOpenSync(s32 arg0, ExecUnk* arg1);

extern u32 UNK_812FD000 AT_ADDRESS(0x812FD000);
extern u32 UNK_812FD008 AT_ADDRESS(0x812FD008);
extern u32 UNK_812FF000 AT_ADDRESS(0x812FF000);

s32 PackArgs(char* arg0, s32 arg1, s32 arg2) {
    // s32 temp_r0;
    s32 temp_r0_2;
    s32 temp_r0_3;
    s32 var_ctr_2;
    s32 var_r26;
    s32* var_r5;
    char* temp_r27;
    u32 var_r27;
    u32 var_r29;
    u32 temp_r31;
    u32 temp_r3;
    u32 temp_r3_2;
    u32 temp_r3_3;
    u32 var_ctr;
    u32 var_r7;
    void* temp_r6;
    // void* var_r3;
    // void* var_r4;
    // void* var_r4_2;

    memset(&UNK_812FD000, 0, 0x2000);

    if (arg1 == -1) {
        UNK_812FD008 = 0;
    } else {
        temp_r31 = arg1 + 1;
        var_r26 = arg1 - 1;
        var_r27 = UNK_812FF000;
        var_r29 = arg2 + (var_r26 * 4);

    loop_4:
        if (var_r26 >= 0) {
            var_r27 -= strlen((char*)&var_r29) + 1;
            strcpy((char*)var_r27, (char*)var_r29);
            var_r29 = var_r27 - UNK_812FD000;
            var_r29 -= 4;
            var_r26 -= 1;
            goto loop_4;
        }
        // temp_r27 = var_r27 - (strlen(arg0) + 1);
        strcpy(temp_r27, arg0);
        temp_r3 = temp_r31 + 1;
        // temp_r6 = (((s32) (temp_r27 + 0x7ED03000) & 0xFFFFFFFC) + UNK_812FD000) - (temp_r3 * 4);
        // temp_r6->unk0 = (s8* ) (temp_r27 + 0x7ED03000);
        var_r7 = 1;
        if (temp_r3 > 1U) {
            temp_r3_2 = temp_r31 - 7;
            if (temp_r31 > 8U) {
                var_ctr = (u32)(temp_r3_2 + 6) >> 3U;
                // var_r3 = arg2 + 4;
                // var_r4 = temp_r6 + 4;
                if (temp_r3_2 > 1U) {
                    do {
                        var_r7 += 8;
                        // var_r4->unk0 = (s32) var_r3->unk-4;
                        // var_r4->unk4 = (s32) var_r3->unk0;
                        // var_r4->unk8 = (s32) var_r3->unk4;
                        // var_r4->unkC = (s32) var_r3->unk8;
                        // var_r4->unk10 = (s32) var_r3->unkC;
                        // var_r4->unk14 = (s32) var_r3->unk10;
                        // var_r4->unk18 = (s32) var_r3->unk14;
                        // temp_r0 = var_r3->unk18;
                        // var_r3 += 0x20;
                        // var_r4->unk1C = temp_r0;
                        // var_r4 += 0x20;
                        var_ctr -= 1;
                    } while (var_ctr != 0);
                }
            }
            temp_r0_2 = var_r7 * 4;
            // var_r4_2 = arg2 + temp_r0_2;
            // var_r5 = temp_r6 + temp_r0_2;
            temp_r3_3 = temp_r31 + 1;
            var_ctr_2 = temp_r3_3 - var_r7;
            if (var_r7 < temp_r3_3) {
                do {
                    // temp_r0_3 = var_r4_2->unk-4;
                    // var_r4_2 += 4;
                    *var_r5 = temp_r0_3;
                    var_r5 += 4;
                    var_ctr_2 -= 1;
                } while (var_ctr_2 != 0);
            }
        }
        // temp_r6->unk-4 = temp_r31;
        UNK_812FD008 = (u32)temp_r6 + 0x7ED02FFC;
    }

    return 1;
}

ASM static void Run(register void* entrypoint) {
#ifdef __MWERKS__ // clang-format off
    fralloc
    bl      OSDisableInterrupts
    bl      ICFlashInvalidate
    sync
    isync   
    mtlr    r31
    blr
#endif // clang-format on
}

void PrepareExecCallback(s32 result, DVDCommandBlock* block) { Prepared = true; }

void PrepareExec(u32 arg0) {
    OSThread* head;
    OSThread* next;

    OSDisableScheduler();
    __OSStopAudioSystem();

    while (__OSCallResetFunctions(false) == false) {}

    OSDisableInterrupts();
    __OSCallResetFunctions(true);
    LCDisable();

    for (head = __OSActiveThreadQueue.head; head != NULL; head = next) {
        next = head->linkActive.next;

        switch (head->state) {
            case 1:
            case 4:
                OSCancelThread(head);
                break;
            default:
                break;
        }
    }

    OSEnableScheduler();
    DVDInit();
    DVDSetAutoInvalidation(true);
    __DVDPrepareResetAsync(PrepareExecCallback);

    if (!DVDCheckDisk()) {
        __OSDoHotReset(0);
    }

    __OSMaskInterrupts(~0x1F);
    __OSUnmaskInterrupts(0x400);
    OSEnableInterrupts();

    while (Prepared == false) {}
}

void ReadCallback(s32 arg0, ExecUnk* arg1) {
    if (arg0 > 0) {
        ReadDone = true;
    }
}

static inline void TGCOpenAsync(s32 result, ExecUnk* pExecUnk) {
    if (THeader.magic != TGC_MAGIC) {
        OSReport("TGCOpenAsync(): Wrong TGC format\n");
        OSPanic("exec.c", 563, "");
    }

    pExecUnk->tgcHeader.headerSize = THeader.headerSize;
    pExecUnk->tgcHeader.unkC = THeader.unkC;
    pExecUnk->tgcHeader.fstOffset = THeader.fstOffset;
    pExecUnk->tgcHeader.fstSize = THeader.fstSize;
    pExecUnk->tgcHeader.fstMaxSize = THeader.fstMaxSize;
    pExecUnk->tgcHeader.bootDolOffset = THeader.bootDolOffset;
    pExecUnk->tgcHeader.bootDolSize = THeader.bootDolSize;
    pExecUnk->tgcHeader.unk24 = THeader.unk24;
    pExecUnk->tgcHeader.unk28 = THeader.unk28;
    pExecUnk->tgcHeader.unk2C = THeader.unk2C;
    pExecUnk->tgcHeader.unk30 = THeader.unk30;
    pExecUnk->tgcHeader.unk34 = THeader.unk34;

    if (pExecUnk->callback != NULL) {
        pExecUnk->callback(result, pExecUnk);
    }
}

void SystemCallback(s32 result, ExecUnk* pExecUnk) {
    if (result != -1) {
        TGCOpenAsync(result, pExecUnk);
    }
}

static inline BOOL unkInline(char* filename, ExecUnk* arg1) {
    if (!DVDOpen(filename, &arg1->fileInfo)) {
        OSReport("TGCOpenAsync(): failed to open %s\n", filename);
        return false;
    }

    if ((arg1->tgcHeader.fstMaxSize % 32) != 0) {
        OSPanic("exec.c", 600, "TGCOpenAsync(): Specified TGC file is not 32KB aligned");
    }

    arg1->callback = cbForTGCOpenSync;
    DVDReadAsyncPrio(&arg1->fileInfo, &THeader, 0x40, 0, (DVDCallback)SystemCallback, 2);
    return true;
}

s32 TGCOpen(char* filename, ExecUnk* arg1) {
    s32 interrupts;
    s32 ret;
    s32 pad[4];

    if (!unkInline(filename, arg1)) {
        return -1;
    }

    interrupts = OSDisableInterrupts();
    while (true) {
        if (arg1->fileInfo.cb.state == 0) {
            ret = 0;
            break;
        } else if (arg1->fileInfo.cb.state == -1) {
            ret = -1;
            break;
        } else if (arg1->fileInfo.cb.state == 10) {
            ret = -3;
            break;
        } else {
            OSSleepThread(&__TGCThreadQueue);
        }
    }
    OSRestoreInterrupts(interrupts);

    return ret;
}

void cbForTGCOpenSync(s32 arg0, ExecUnk* arg1) { OSWakeupThread(&__TGCThreadQueue); }

void __TgcExec(int nCount, char** aszArgument);

void TGCExec(ExecUnk* arg1, int nCount, char** aszArgument) {
    char** var_r6;

    if (aszArgument == NULL) {
        __TgcExec(-1, NULL);
    } else {
        var_r6 = aszArgument;

        while (*var_r6 != 0) {
            var_r6++;
            nCount++;
        }

        // loop_5:
        //         nCount++;
        //         var_r6++;
        //     if (*var_r6 != 0) {
        //         goto loop_5;
        //     }

        __TgcExec(nCount, var_r6);
    }
}
