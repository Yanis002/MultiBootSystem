#ifndef _DOLPHIN_OSALLOC
#define _DOLPHIN_OSALLOC

#ifdef __cplusplus
extern "C" {
#endif

#include "dolphin/types.h"

typedef int OSHeapHandle;
typedef void (*OSAllocVisitor)(void* obj, u32 size);

void* OSAllocFromHeap(OSHeapHandle heap, u32 size);
void OSFreeToHeap(OSHeapHandle heap, void* ptr);
OSHeapHandle OSSetCurrentHeap(OSHeapHandle heap);
void* OSInitAlloc(void* arenaStart, void* arenaEnd, int maxHeaps);
OSHeapHandle OSCreateHeap(void* start, void* end);
s32 OSCheckHeap(OSHeapHandle heap);
s32 OSReferentSize(void* ptr);
void OSDumpHeap(OSHeapHandle handle);

extern volatile OSHeapHandle __OSCurrHeap;

#define OSAlloc(size) OSAllocFromHeap(__OSCurrHeap, (size))
#define OSFree(ptr) OSFreeToHeap(__OSCurrHeap, (ptr))

#ifdef __cplusplus
};
#endif

#endif // _DOLPHIN_OSALLOC
