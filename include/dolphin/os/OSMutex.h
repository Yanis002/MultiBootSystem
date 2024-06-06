#ifndef _DOLPHIN_OSMUTEX
#define _DOLPHIN_OSMUTEX

#ifdef __cplusplus
extern "C" {
#endif

#include "dolphin/os/OSThread.h"
#include "dolphin/types.h"

struct OSMutex {
    OSThreadQueue queue;
    OSThread* thread; // the current owner
    s32 count; // lock count
    OSMutexLink link; // for OSThread.queueMutex
};

struct OSCond {
    OSThreadQueue queue;
};

void OSLockMutex(OSMutex* mutex);
void OSUnlockMutex(OSMutex* mutex);

void __OSUnlockAllMutex(OSThread* thread);

#ifdef __cplusplus
};
#endif

#endif // _DOLPHIN_OSMUTEX
