#ifndef ARQ_H
#define ARQ_H

#include "dolphin/types.h"

#ifdef __cplusplus
extern "C" {
#endif

#define ARQ_CHUNK_SIZE_DEFAULT 4096

typedef void (*ARQCallback)(u32 ptrToRequest);

typedef enum _ARamType {
    ARAM_DIR_MRAM_TO_ARAM,
    ARAM_DIR_ARAM_TO_MRAM,
} ARamType;

typedef enum _ArqPriotity {
    ARQ_PRIORITY_LOW,
    ARQ_PRIORITY_HIGH,
} ArqPriotity;

typedef struct ARQRequest {
    /* 0x00 */ struct ARQRequest* next;
    /* 0x04 */ u32 owner;
    /* 0x08 */ u32 type;
    /* 0x0C */ u32 priority;
    /* 0x10 */ u32 source;
    /* 0x14 */ u32 dest;
    /* 0x18 */ u32 length;
    /* 0x1C */ ARQCallback callback;
} ARQRequest;

void ARQInit(void);
void ARQReset(void);
void ARQPostRequest(ARQRequest* request, u32 owner, u32 type, u32 priority, u32 source, u32 dest, u32 length,
                    ARQCallback callback);
void ARQRemoveRequest(ARQRequest* request);
void ARQRemoveOwnerRequest(u32 owner);
void ARQFlushQueue(void);
void ARQSetChunkSize(u32 size);
u32 ARQGetChunkSize(void);

#ifdef __cplusplus
};
#endif

#endif /* ARQ_H */
