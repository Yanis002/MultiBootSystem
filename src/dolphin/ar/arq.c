#include "dolphin/ar.h"

const char* __ARQVersion = "<< Dolphin SDK - ARQ\trelease build: Sep  5 2002 05:34:29 (0x2301) >>";

static ARQRequest* __ARQRequestQueueHi;
static ARQRequest* __ARQRequestTailHi;
static ARQRequest* __ARQRequestQueueLo;
static ARQRequest* __ARQRequestTailLo;
#ifdef UNUSED
static ARQRequest* __ARQRequestQueueTemp;
static ARQRequest* __ARQRequestTailTemp;
#endif
static ARQRequest* __ARQRequestPendingHi;
static ARQRequest* __ARQRequestPendingLo;
static ARQCallback __ARQCallbackHi;
static ARQCallback __ARQCallbackLo;
static u32 __ARQChunkSize;
static int __ARQ_init_flag;

void __ARQPopTaskQueueHi(void) {
    if (__ARQRequestQueueHi) {
        if (__ARQRequestQueueHi->type == 0) {
            ARStartDMA(__ARQRequestQueueHi->type, __ARQRequestQueueHi->source, __ARQRequestQueueHi->dest,
                       __ARQRequestQueueHi->length);
        } else {
            ARStartDMA(__ARQRequestQueueHi->type, __ARQRequestQueueHi->dest, __ARQRequestQueueHi->source,
                       __ARQRequestQueueHi->length);
        }
        __ARQCallbackHi = __ARQRequestQueueHi->callback;
        __ARQRequestPendingHi = __ARQRequestQueueHi;
        __ARQRequestQueueHi = __ARQRequestQueueHi->next;
    }
}

void __ARQServiceQueueLo(void) {
    if (__ARQRequestPendingLo == 0 && __ARQRequestQueueLo) {
        __ARQRequestPendingLo = __ARQRequestQueueLo;
        __ARQRequestQueueLo = __ARQRequestQueueLo->next;
    }
    if (__ARQRequestPendingLo) {
        if (__ARQRequestPendingLo->length <= __ARQChunkSize) {
            if (__ARQRequestPendingLo->type == 0) {
                ARStartDMA(__ARQRequestPendingLo->type, __ARQRequestPendingLo->source, __ARQRequestPendingLo->dest,
                           __ARQRequestPendingLo->length);
            } else {
                ARStartDMA(__ARQRequestPendingLo->type, __ARQRequestPendingLo->dest, __ARQRequestPendingLo->source,
                           __ARQRequestPendingLo->length);
            }
            __ARQCallbackLo = __ARQRequestPendingLo->callback;
        } else if (__ARQRequestPendingLo->type == 0) {
            ARStartDMA(__ARQRequestPendingLo->type, __ARQRequestPendingLo->source, __ARQRequestPendingLo->dest,
                       __ARQChunkSize);
        } else {
            ARStartDMA(__ARQRequestPendingLo->type, __ARQRequestPendingLo->dest, __ARQRequestPendingLo->source,
                       __ARQChunkSize);
        }
        __ARQRequestPendingLo->length -= __ARQChunkSize;
        __ARQRequestPendingLo->source += __ARQChunkSize;
        __ARQRequestPendingLo->dest += __ARQChunkSize;
    }
}

void __ARQCallbackHack(u32 unused) {}

void __ARQInterruptServiceRoutine() {
    if (__ARQCallbackHi) {
        __ARQCallbackHi((u32)__ARQRequestPendingHi);
        __ARQRequestPendingHi = NULL;
        __ARQCallbackHi = NULL;
    } else if (__ARQCallbackLo) {
        __ARQCallbackLo((u32)__ARQRequestPendingLo);
        __ARQRequestPendingLo = NULL;
        __ARQCallbackLo = NULL;
    }
    __ARQPopTaskQueueHi();
    if (__ARQRequestPendingHi == 0) {
        __ARQServiceQueueLo();
    }
}

#ifdef UNUSED

void __ARQInitTempQueue(void) {
    __ARQRequestQueueTemp = NULL;
    __ARQRequestTailTemp = NULL;
}

void __ARQPushTempQueue(ARQRequest* task) {
    if (!__ARQRequestQueueTemp) {
        __ARQRequestQueueTemp = task;
        __ARQRequestTailTemp = task;
        return;
    }
    __ARQRequestTailTemp->next = task;
    __ARQRequestTailTemp = task;
}

#endif

void ARQInit(void) {
    if (__ARQ_init_flag == true) {
        return;
    }

    OSRegisterVersion(__ARQVersion);
    __ARQRequestQueueHi = __ARQRequestQueueLo = NULL;
    __ARQChunkSize = 0x1000;
    ARRegisterDMACallback(__ARQInterruptServiceRoutine);
    __ARQRequestPendingHi = NULL;
    __ARQRequestPendingLo = NULL;
    __ARQCallbackHi = NULL;
    __ARQCallbackLo = NULL;
    __ARQ_init_flag = true;
}

void ARQReset(void) { __ARQ_init_flag = 0; }

void ARQPostRequest(ARQRequest* request, u32 owner, u32 type, u32 priority, u32 source, u32 dest, u32 length,
                    ARQCallback callback) {
    int level;

    request->next = NULL;
    request->owner = owner;
    request->type = type;
    request->source = source;
    request->dest = dest;
    request->length = length;
    if (callback) {
        request->callback = callback;
    } else {
        request->callback = __ARQCallbackHack;
    }
    level = OSDisableInterrupts();
    switch (priority) {
        case ARQ_PRIORITY_LOW:
            if (__ARQRequestQueueLo) {
                __ARQRequestTailLo->next = request;
            } else {
                __ARQRequestQueueLo = request;
            }
            __ARQRequestTailLo = request;
            break;
        case ARQ_PRIORITY_HIGH:
            if (__ARQRequestQueueHi) {
                __ARQRequestTailHi->next = request;
            } else {
                __ARQRequestQueueHi = request;
            }
            __ARQRequestTailHi = request;
            break;
    }
    if ((__ARQRequestPendingHi == 0) && (__ARQRequestPendingLo == 0)) {
        __ARQPopTaskQueueHi();
        if (__ARQRequestPendingHi == 0) {
            __ARQServiceQueueLo();
        }
    }
    OSRestoreInterrupts(level);
}

#ifdef UNUSED

void ARQRemoveRequest(ARQRequest* request) {
    ARQRequest* thisRequest;
    int level;

    level = OSDisableInterrupts();
    __ARQInitTempQueue();
    for (thisRequest = __ARQRequestQueueHi; thisRequest; thisRequest = thisRequest->next) {
        if (thisRequest != request) {
            __ARQPushTempQueue(thisRequest);
        }
    }
    __ARQRequestQueueHi = __ARQRequestQueueTemp;
    __ARQRequestTailHi = __ARQRequestTailTemp;
    __ARQInitTempQueue();
    for (thisRequest = __ARQRequestQueueLo; thisRequest; thisRequest = thisRequest->next) {
        if (thisRequest != request) {
            __ARQPushTempQueue(thisRequest);
        }
    }
    __ARQRequestQueueLo = __ARQRequestQueueTemp;
    __ARQRequestTailLo = __ARQRequestTailTemp;
    if ((__ARQRequestPendingLo) && (__ARQRequestPendingLo == request) && (__ARQCallbackLo == 0)) {
        __ARQRequestPendingLo = NULL;
    }
    OSRestoreInterrupts(level);
}

void ARQRemoveOwnerRequest(u32 owner) {
    ARQRequest* thisRequest;
    int level;

    level = OSDisableInterrupts();
    __ARQInitTempQueue();
    for (thisRequest = __ARQRequestQueueHi; thisRequest; thisRequest = thisRequest->next) {
        if (thisRequest->owner != owner) {
            __ARQPushTempQueue(thisRequest);
        }
    }
    __ARQRequestQueueHi = __ARQRequestQueueTemp;
    __ARQRequestTailHi = __ARQRequestTailTemp;
    __ARQInitTempQueue();
    for (thisRequest = __ARQRequestQueueLo; thisRequest; thisRequest = thisRequest->next) {
        if (thisRequest->owner != owner) {
            __ARQPushTempQueue(thisRequest);
        }
    }
    __ARQRequestQueueLo = __ARQRequestQueueTemp;
    __ARQRequestTailLo = __ARQRequestTailTemp;
    if ((__ARQRequestPendingLo) && (__ARQRequestPendingLo->owner == owner) && (__ARQCallbackLo == 0)) {
        __ARQRequestPendingLo = NULL;
    }
    OSRestoreInterrupts(level);
}

#endif

void ARQFlushQueue(void) {
    int level;

    level = OSDisableInterrupts();
    __ARQRequestQueueHi = NULL;
    __ARQRequestTailHi = NULL;
    __ARQRequestQueueLo = NULL;
    __ARQRequestTailLo = NULL;
    if (__ARQCallbackLo == 0) {
        __ARQRequestPendingLo = NULL;
    }
    OSRestoreInterrupts(level);
}

void ARQSetChunkSize(u32 size) {
    u32 i;

    i = size & 0x1F;
    if (i) {
        __ARQChunkSize = size + (0x20 - i);
        return;
    }
    __ARQChunkSize = size;
}

u32 ARQGetChunkSize(void) { return __ARQChunkSize; }
