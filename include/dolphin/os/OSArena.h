#ifndef _DOLPHIN_OSARENA
#define _DOLPHIN_OSARENA

#ifdef __cplusplus
extern "C" {
#endif

#include "dolphin/types.h"

void* OSGetArenaHi(void);
void* OSGetArenaLo(void);
void OSSetArenaHi(void* addr);
void OSSetArenaLo(void* addr);

#ifdef __cplusplus
};
#endif

#endif // _DOLPHIN_OSARENA
