#include "dolphin/dvd.h"
#include "dolphin/os.h"
#include "dolphin/types.h"
#include "stddef.h"

typedef struct FSTEntry FSTEntry;

struct FSTEntry {
    u32 isDirAndStringOff;
    u32 parentOrPosition;
    u32 nextEntryOrLength;
};

static OSBootInfo* BootInfo;
static FSTEntry* FstStart;
static char* FstStringStart;
static u32 MaxEntryNum;
static u32 currentDirectory = 0;
OSThreadQueue __DVDThreadQueue;
u32 __DVDLongFileNameFlag = 0;

static void cbForReadAsync(s32 result, DVDCommandBlock* block);
static void cbForReadSync(s32 result, DVDCommandBlock* block);
static void cbForPrepareStreamAsync(s32 result, DVDCommandBlock* block);

void __DVDFSInit() {
    BootInfo = (OSBootInfo*)OSPhysicalToCached(0);
    FstStart = (FSTEntry*)BootInfo->FSTLocation;

    if (FstStart) {
        MaxEntryNum = FstStart[0].nextEntryOrLength;
        FstStringStart = (char*)&(FstStart[MaxEntryNum]);
    }
}

/* For convenience */
#define entryIsDir(i) (((FstStart[i].isDirAndStringOff & 0xFF000000) == 0) ? false : true)
#define stringOff(i) (FstStart[i].isDirAndStringOff & ~0xFF000000)
#define parentDir(i) (FstStart[i].parentOrPosition)
#define nextDir(i) (FstStart[i].nextEntryOrLength)
#define filePosition(i) (FstStart[i].parentOrPosition)
#define fileLength(i) (FstStart[i].nextEntryOrLength)
#define Is32KBAligned(x) (((u32)(x) & (32 * 1024 - 1)) == 0)

static inline BOOL isSame(const char* path, const char* string) {
    while (*string != '\0') {
        if (tolower(*path++) != tolower(*string++)) {
            return false;
        }
    }

    if ((*path == '/') || (*path == '\0')) {
        return true;
    }

    return false;
}

s32 DVDConvertPathToEntrynum(const char* pathPtr) {
    const char* ptr;
    char* stringPtr;
    BOOL isDir;
    u32 length;
    u32 dirLookAt;
    u32 i;
    const char* origPathPtr = pathPtr;
    const char* extentionStart;
    BOOL illegal;
    BOOL extention;

    dirLookAt = currentDirectory;

    while (1) {

        if (*pathPtr == '\0') {
            return (s32)dirLookAt;
        } else if (*pathPtr == '/') {
            dirLookAt = 0;
            pathPtr++;
            continue;
        } else if (*pathPtr == '.') {
            if (*(pathPtr + 1) == '.') {
                if (*(pathPtr + 2) == '/') {
                    dirLookAt = parentDir(dirLookAt);
                    pathPtr += 3;
                    continue;
                } else if (*(pathPtr + 2) == '\0') {
                    return (s32)parentDir(dirLookAt);
                }
            } else if (*(pathPtr + 1) == '/') {
                pathPtr += 2;
                continue;
            } else if (*(pathPtr + 1) == '\0') {
                return (s32)dirLookAt;
            }
        }

        if (__DVDLongFileNameFlag == 0) {
            extention = false;
            illegal = false;

            for (ptr = pathPtr; (*ptr != '\0') && (*ptr != '/'); ptr++) {
                if (*ptr == '.') {
                    if ((ptr - pathPtr > 8) || (extention == true)) {
                        illegal = true;
                        break;
                    }
                    extention = true;
                    extentionStart = ptr + 1;

                } else if (*ptr == ' ') {
                    illegal = true;
                }
            }

            if ((extention == true) && (ptr - extentionStart > 3)) {
                illegal = true;
            }

            if (illegal) {
                OSPanic("dvdfs.c", IS_MQ ? 379 : 383,
                        "DVDConvertEntrynumToPath(possibly DVDOpen or DVDChangeDir or DVDOpenDir): "
                        "specified directory or file (%s) doesn't match standard 8.3 format. This is a "
                        "temporary restriction and will be removed soon\n",
                        origPathPtr);
            }
        } else {
            for (ptr = pathPtr; (*ptr != '\0') && (*ptr != '/'); ptr++)
                ;
        }

        isDir = (*ptr == '\0') ? false : true;
        length = (u32)(ptr - pathPtr);

        ptr = pathPtr;

        for (i = dirLookAt + 1; i < nextDir(dirLookAt); i = entryIsDir(i) ? nextDir(i) : (i + 1)) {
            if ((entryIsDir(i) == false) && (isDir == true)) {
                continue;
            }

            stringPtr = FstStringStart + stringOff(i);

            if (isSame(ptr, stringPtr) == true) {
                goto next_hier;
            }
        }

        return -1;

    next_hier:
        if (!isDir) {
            return (s32)i;
        }

        dirLookAt = i;
        pathPtr += length + 1;
    }
}

BOOL DVDFastOpen(s32 entrynum, DVDFileInfo* fileInfo) {
    if ((entrynum < 0) || (entrynum >= MaxEntryNum) || entryIsDir(entrynum)) {
        return false;
    }

    fileInfo->startAddr = filePosition(entrynum);
    fileInfo->length = fileLength(entrynum);
    fileInfo->callback = (DVDCallback)NULL;
    fileInfo->cb.state = DVD_STATE_END;

    return true;
}

BOOL DVDOpen(const char* fileName, DVDFileInfo* fileInfo) {
    s32 entry;
    char currentDir[128];

    entry = DVDConvertPathToEntrynum(fileName);

    if (0 > entry) {
        DVDGetCurrentDir(currentDir, 128);
        OSReport("Warning: DVDOpen(): file '%s' was not found under %s.\n", fileName, currentDir);
        return false;
    }

    if (entryIsDir(entry)) {
        return false;
    }

    fileInfo->startAddr = filePosition(entry);
    fileInfo->length = fileLength(entry);
    fileInfo->callback = (DVDCallback)NULL;
    fileInfo->cb.state = DVD_STATE_END;

    return true;
}

BOOL DVDClose(DVDFileInfo* fileInfo) {
    DVDCancel(&(fileInfo->cb));
    return true;
}

static inline u32 myStrncpy(char* dest, char* src, u32 maxlen) {
    u32 i = maxlen;

    while ((i > 0) && (*src != 0)) {
        *dest++ = *src++;
        i--;
    }

    return (maxlen - i);
}

static u32 entryToPath(u32 entry, char* path, u32 maxlen) {
    char* name;
    u32 loc;

    if (entry == 0) {
        return 0;
    }

    name = FstStringStart + stringOff(entry);

    loc = entryToPath(parentDir(entry), path, maxlen);

    if (loc == maxlen) {
        return loc;
    }

    *(path + loc++) = '/';

    loc += myStrncpy(path + loc, name, maxlen - loc);

    return loc;
}

static inline BOOL DVDConvertEntrynumToPath(s32 entrynum, char* path, u32 maxlen) {
    u32 loc;

    loc = entryToPath((u32)entrynum, path, maxlen);

    if (loc == maxlen) {
        path[maxlen - 1] = '\0';
        return false;
    }

    if (entryIsDir(entrynum)) {
        if (loc == maxlen - 1) {
            path[loc] = '\0';
            return false;
        }

        path[loc++] = '/';
    }

    path[loc] = '\0';
    return true;
}

BOOL DVDGetCurrentDir(char* path, u32 maxlen) { return DVDConvertEntrynumToPath((s32)currentDirectory, path, maxlen); }

BOOL DVDReadAsyncPrio(DVDFileInfo* fileInfo, void* addr, s32 length, s32 offset, DVDCallback callback, s32 prio) {

    if (!((0 <= offset) && (offset < fileInfo->length))) {
        OSPanic("dvdfs.c", IS_MQ ? 742 : 746, "DVDReadAsync(): specified area is out of the file  ");
    }

    if (!((0 <= offset + length) && (offset + length < fileInfo->length + DVD_MIN_TRANSFER_SIZE))) {
        OSPanic("dvdfs.c", IS_MQ ? 748 : 752, "DVDReadAsync(): specified area is out of the file  ");
    }

    fileInfo->callback = callback;
    DVDReadAbsAsyncPrio(&(fileInfo->cb), addr, length, (s32)(fileInfo->startAddr + offset), cbForReadAsync, prio);

    return true;
}

static void cbForReadAsync(s32 result, DVDCommandBlock* block) {
    DVDFileInfo* fileInfo;

    fileInfo = (DVDFileInfo*)((char*)block - OFFSETOF(DVDFileInfo, cb));
    if (fileInfo->callback) {
        (fileInfo->callback)(result, fileInfo);
    }
}

s32 DVDReadPrio(DVDFileInfo* fileInfo, void* addr, s32 length, s32 offset, s32 prio) {
    BOOL result;
    DVDCommandBlock* block;
    s32 state;
    BOOL enabled;
    s32 retVal;

    if (!((0 <= offset) && (offset < fileInfo->length))) {
        OSPanic("dvdfs.c", IS_MQ ? 812 : 816, "DVDRead(): specified area is out of the file  ");
    }

    if (!((0 <= offset + length) && (offset + length < fileInfo->length + DVD_MIN_TRANSFER_SIZE))) {
        OSPanic("dvdfs.c", IS_MQ ? 818 : 822, "DVDRead(): specified area is out of the file  ");
    }

    block = &(fileInfo->cb);

    result = DVDReadAbsAsyncPrio(block, addr, length, (s32)(fileInfo->startAddr + offset), cbForReadSync, prio);

    if (result == false) {
        return -1;
    }

    enabled = OSDisableInterrupts();

    while (true) {
        state = ((volatile DVDCommandBlock*)block)->state;

        if (state == DVD_STATE_END) {
            retVal = (s32)block->transferredSize;
            break;
        }
        if (state == DVD_STATE_FATAL_ERROR) {
            retVal = DVD_RESULT_FATAL_ERROR;
            break;
        }
        if (state == DVD_STATE_CANCELED) {
            retVal = DVD_RESULT_CANCELED;
            break;
        }

        OSSleepThread(&__DVDThreadQueue);
    }

    OSRestoreInterrupts(enabled);
    return retVal;
}

static inline void cbForReadSync(s32 result, DVDCommandBlock* block) { OSWakeupThread(&__DVDThreadQueue); }

// strings from unused functions (``DVDSeekAsyncPrio`` and ``DVDOpenDir``)
static char unused1[] = "DVDSeek(): offset is out of the file  ";
static char unused2[] = "Warning: DVDOpenDir(): file '%s' was not found under %s.\n";

BOOL DVDPrepareStreamAsync(DVDFileInfo* fileInfo, u32 length, u32 offset, DVDCallback callback) {
    u32 start;

    start = fileInfo->startAddr + offset;

    if (!Is32KBAligned(start)) {
        OSPanic("dvdfs.c", 1189,
                "DVDPrepareStreamAsync(): Specified start address (filestart(0x%x) + offset(0x%x)) is "
                "not 32KB aligned",
                fileInfo->startAddr, offset);
    }

    if (length == 0) {
        length = fileInfo->length - offset;
    }

    if (!Is32KBAligned(length)) {
        OSPanic("dvdfs.c", 1199, "DVDPrepareStreamAsync(): Specified length (0x%x) is not a multiple of 32768(32*1024)",
                length);
    }

    if (!((offset < fileInfo->length) && (offset + length <= fileInfo->length))) {
        OSPanic("dvdfs.c", 1207,
                "DVDPrepareStreamAsync(): The area specified (offset(0x%x), length(0x%x)) is out of "
                "the file",
                offset, length);
    }

    fileInfo->callback = callback;
    return DVDPrepareStreamAbsAsync(&(fileInfo->cb), length, fileInfo->startAddr + offset, cbForPrepareStreamAsync);
}

static void cbForPrepareStreamAsync(s32 result, DVDCommandBlock* block) {
    DVDFileInfo* fileInfo;

    fileInfo = (DVDFileInfo*)((char*)block - OFFSETOF(DVDFileInfo, cb));

    if (fileInfo->callback) {
        (fileInfo->callback)(result, fileInfo);
    }
}

// strings from unused function ``DVDPrepareStream``
static char unused3[] =
    "DVDPrepareStream(): Specified start address (filestart(0x%x) + offset(0x%x)) is not 32KB aligned";
static char unused4[] = "DVDPrepareStream(): Specified length (0x%x) is not a multiple of 32768(32*1024)";
static char unused5[] = "DVDPrepareStream(): The area specified (offset(0x%x), length(0x%x)) is out of the file";
