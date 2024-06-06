#ifndef JAIINITDATA_H
#define JAIINITDATA_H

#include "dolphin/types.h"

namespace JAInter {
    namespace InitData {
        BOOL checkInitDataFile();
        void checkInitDataOnMemory();

        extern u32* aafPointer;
    }; // namespace InitData
} // namespace JAInter

#endif /* JAIINITDATA_H */
