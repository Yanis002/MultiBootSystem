#include "dolphin/types.h"
#include "dolphin/dtk.h"
#include "dolphin/ai.h"

DTKTrack __Free_Tracks[32];
static u32 free_track_index;

void Init_DTK_System() {
    AIInit(0);
    DTKInit();
}

u32 Add_Track(char* filename, DTKCallback callback) {
    if (free_track_index < ARRAY_COUNT(__Free_Tracks)) {
        u32 ret = DTKQueueTrack(filename, &__Free_Tracks[free_track_index], 0x3F, callback);
        free_track_index += 1;
        return ret;
    }

    return 4;
}

void Play_Track() {
    if (DTKGetCurrentTrack() != 0) {
        while (DTKGetState() == 3) {}
        DTKSetState(1);
    }
}

void Stop_Track() {
    if (DTKGetCurrentTrack() != 0) {
        while (DTKGetState() == 3) {}
        DTKSetState(0);
    }
}
