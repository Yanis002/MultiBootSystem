#ifndef _EXEC_H_
#define _EXEC_H_

#include "dolphin/dvd.h"
#include "dolphin/types.h"

#ifdef __cplusplus
extern "C" {
#endif

#define TGC_MAGIC 0xAE0F38A2

typedef void (*ExecCallback)(s32 arg0, struct ExecUnk* arg1);

typedef struct TGCHeader {
    /* 0x00 */ u32 magic; // see TGC_MAGIC
    /* 0x04 */ u32 unk4; // usually 0x00000000
    /* 0x08 */ u32 headerSize; // usually 0x00008000
    /* 0x0C */ u32 unkC; // usually 0x00100000
    /* 0x10 */ u32 fstOffset;
    /* 0x14 */ u32 fstSize;
    /* 0x18 */ u32 fstMaxSize;
    /* 0x1C */ u32 bootDolOffset;
    /* 0x20 */ u32 bootDolSize;
    /* 0x24 */ u32 unk24;
    /* 0x28 */ u32 unk28;
    /* 0x2C */ u32 unk2C;
    /* 0x30 */ u32 unk30;
    /* 0x34 */ u32 unk34;
} TGCHeader; // size = 0x38

typedef struct ExecUnk {
    /* 0x00 */ TGCHeader tgcHeader;
    /* 0x38 */ ExecCallback callback;
    /* 0x3C */ DVDFileInfo fileInfo;
} ExecUnk;

s32 TGCOpen(char* filename, struct ExecUnk* arg1);
void TGCExec(ExecUnk* arg1, int nCount, char** aszArgument);

#ifdef __cplusplus
};
#endif

#endif
