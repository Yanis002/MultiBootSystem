#ifndef _DOLPHIN_OSALARM
#define _DOLPHIN_OSALARM

#ifdef __cplusplus
extern "C" {
#endif

#include "dolphin/os/OSContext.h"
#include "dolphin/os/OSTime.h"
#include "dolphin/types.h"

typedef struct OSAlarm OSAlarm;
typedef void (*OSAlarmHandler)(OSAlarm* alarm, OSContext* context);

struct OSAlarm {
    OSAlarmHandler handler;
    u32 tag;
    OSTime fire;
    OSAlarm* prev;
    OSAlarm* next;
    OSTime period;
    OSTime start;
};

void OSInitAlarm(void);
void OSCreateAlarm(OSAlarm* alarm);
void OSSetAlarm(OSAlarm* alarm, OSTime tick, OSAlarmHandler handler);
void OSCancelAlarm(OSAlarm* alarm);

BOOL OSCheckAlarmQueue(void);

#ifdef __cplusplus
};
#endif

#endif // _DOLPHIN_OSALARM
