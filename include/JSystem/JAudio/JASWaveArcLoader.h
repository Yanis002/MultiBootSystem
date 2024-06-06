#ifndef JASWAVEARCLOADER_H
#define JASWAVEARCLOADER_H

#include "JSystem/JAudio/JASHeapCtrl.h"
#include "dolphin/os/OSMutex.h"

namespace JASystem {
    namespace WaveArcLoader {
        bool init();
        Kernel::THeap* getRootHeap();
        void setCurrentDir(const char*);
        char* getCurrentDir();

        extern Kernel::THeap sAramHeap;
        extern char sCurrentDir[];
    }; // namespace WaveArcLoader

    class TWaveArc : public Kernel::TDisposer {
      public:
        TWaveArc();
        virtual void onLoadDone() {}
        virtual void onEraseDone() {}
        bool loadSetup(u32);
        bool eraseSetup();
        static s32 loadToAramCallback(void*);
        bool sendLoadCmd();
        bool load(Kernel::THeap*);
        bool erase();
        void onDispose();
        void setEntryNum(s32);
        void setFileName(const char*);

        /* 0x04 */ Kernel::THeap mHeap;
        /* 0x4C */ int field_0x4c;
        /* 0x50 */ OSMutex mMutex;
        /* 0x68 */ int field_0x68;
        /* 0x6C */ int mFileNo;
        /* 0x70 */ int mSize;
        /* 0x74 */ int field_0x74;
    };
} // namespace JASystem

#endif /* JASWAVEARCLOADER_H */
