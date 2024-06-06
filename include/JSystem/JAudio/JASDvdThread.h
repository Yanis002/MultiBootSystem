#ifndef JASDVDTHREAD_H
#define JASDVDTHREAD_H

#include "JSystem/JKernel/JKRThread.h"

class JASTaskThread : public JKRThread {
  public:
    class TCallStack {
      public:
        TCallStack(u32);
        ~TCallStack();

        /* 0x00 */ s32 (*field_0x0)(void*);
        /* 0x04 */ u8* argspace;
    };

    JASTaskThread(u32 param_1, int param_2, s32 param_3) : JKRThread(param_1, param_2, param_3) {
        mPaused = 0;
        OSInitThreadQueue(&mQ);
    }
    ~JASTaskThread();
    BOOL sendCmdMsg(s32 (*)(void*), void*, u32);
    void* run();
    void pause(bool);

    /* 0x68 */ OSThreadQueue mQ;
    /* 0x70 */ u8 mPaused;
};

namespace JASystem {
    namespace Dvd {
        bool createThread(int, int, u32);
        void resumeThread();
        BOOL sendCmdMsg(s32 (*)(void*), void*, u32);
        int checkPassDvdT(u32, u32*, void (*)(u32));
        u32 checkFile(char*);
        void unpauseDvdT();
        s32 dvdThreadCheckBack(void*);

        extern JASTaskThread* sThread;
    } // namespace Dvd
} // namespace JASystem

#endif /* JASDVDTHREAD_H */
