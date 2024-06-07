#include "dolphin/os.h"

#define PopHead(queue, mutex, link)   \
    do {                              \
        OSMutex* __next;              \
                                      \
        (mutex) = (queue)->head;      \
        __next = (mutex)->link.next;  \
        if (__next == NULL)           \
            (queue)->tail = NULL;     \
        else                          \
            __next->link.prev = NULL; \
        (queue)->head = __next;       \
    } while (0)

#define PopMutex(queue, mutex, link)                 \
    do {                                             \
        struct OSMutex* __next = (mutex)->link.next; \
        struct OSMutex* __prev = (mutex)->link.prev; \
        if (__next == NULL) {                        \
            (queue)->tail = __prev;                  \
        } else {                                     \
            __next->link.prev = __prev;              \
        }                                            \
        if (__prev == NULL) {                        \
            (queue)->head = __next;                  \
        } else {                                     \
            __prev->link.next = __next;              \
        }                                            \
    } while (0);

#define PushMutex(queue, mutex, link)           \
    do {                                        \
        struct OSMutex* __prev = (queue)->tail; \
        if (__prev == NULL) {                   \
            (queue)->head = (mutex);            \
        } else {                                \
            __prev->link.next = (mutex);        \
        }                                       \
        (mutex)->link.prev = __prev;            \
        (mutex)->link.next = 0;                 \
        (queue)->tail = (mutex);                \
    } while (0);

void OSInitMutex(OSMutex* mutex) {
    OSInitThreadQueue(&mutex->queue);
    mutex->thread = 0;
    mutex->count = 0;
}

void OSLockMutex(OSMutex* mutex) {
    int enabled = OSDisableInterrupts();
    OSThread* currentThread = OSGetCurrentThread();

    while (true) {
        OSThread* ownerThread = mutex->thread;

        if (ownerThread == 0) {
            mutex->thread = currentThread;
            mutex->count++;
            PushMutex(&currentThread->queueMutex, mutex, link);
            break;
        } else if (ownerThread == currentThread) {
            mutex->count++;
            break;
        } else {
            currentThread->mutex = mutex;
            __OSPromoteThread(mutex->thread, currentThread->priority);
            OSSleepThread(&mutex->queue);
            currentThread->mutex = NULL;
        }
    }

    OSRestoreInterrupts(enabled);
}

void OSUnlockMutex(OSMutex* mutex) {
    int enabled = OSDisableInterrupts();
    OSThread* currentThread = OSGetCurrentThread();

    if (mutex->thread == currentThread) {
        if (!--mutex->count) {
            PopMutex(&currentThread->queueMutex, mutex, link);
            mutex->thread = 0;

            if (currentThread->priority < currentThread->base) {
                currentThread->priority = __OSGetEffectivePriority(currentThread);
            }
            OSWakeupThread(&mutex->queue);
        }
    }

    OSRestoreInterrupts(enabled);
}

void __OSUnlockAllMutex(OSThread* thread) {
    OSMutex* mutex;

    while (thread->queueMutex.head) {
        PopHead(&thread->queueMutex, mutex, link);
        mutex->count = 0;
        mutex->thread = NULL;
        OSWakeupThread(&mutex->queue);
    }
}
