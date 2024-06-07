#ifndef _DOLPHIN_AM_H
#define _DOLPHIN_AM_H

#include "dolphin/types.h"

#ifdef __cplusplus
extern "C" {
#endif

#define AM_STACK_ENTRIES 16
#define AM_ZEROBUFFER_BYTES 256
#define AMPushBuffered(p, b, s) __AMPushBuffered(p, b, s, NULL, FALSE)
#define AMPushBufferedAsync(p, b, s, cb) __AMPushBuffered(p, b, s, cb, TRUE)

typedef void (*AMCallback)(char* path);

void AMInit(u32 aramBase, u32 aramBytes);
u32 AMGetZeroBuffer(void);
u32 AMGetReadStatus(void);
u32 AMGetFreeSize(void);
u32 AMGetStackPointer(void);
void* AMLoadFile(char* path, u32* length);
void AMPop(void);
u32 AMPush(char* path);
u32 AMPushData(void* buffer, u32 length);
u32 __AMPushBuffered(char* path, void* buffer, u32 buffer_size, AMCallback callback, BOOL async_flag);

#ifdef __cplusplus
}
#endif

#endif
