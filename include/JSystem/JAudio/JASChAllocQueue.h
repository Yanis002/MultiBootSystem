#ifndef JASCHALLOCQUEUE_H
#define JASCHALLOCQUEUE_H

#include "JSystem/JSupport/JSUList.h"

namespace JASystem {
    class TChannel;
    namespace TDSPQueue {
        void deQueue();
        void enQueue(JASystem::TChannel*);
        int deleteQueue(JASystem::TChannel*);
        void checkQueue();

    } // namespace TDSPQueue

    extern JSUList<TChannel> sDspQueueList;
} // namespace JASystem

#endif /* JASCHALLOCQUEUE_H */
