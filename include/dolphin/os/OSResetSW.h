#ifndef _DOLPHIN_OSRESETSW
#define _DOLPHIN_OSRESETSW

#ifdef __cplusplus
extern "C" {
#endif

#include "dolphin/os/OSContext.h"
#include "dolphin/types.h"

typedef void (*OSResetCallback)(void);

BOOL OSGetResetButtonState(void);

#ifdef __cplusplus
};
#endif

#endif // _DOLPHIN_OSRESETSW
