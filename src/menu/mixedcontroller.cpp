#include "dolphin/types.h"
#include "dolphin/pad.h"

PADStatus s_pad[PAD_MAX_CONTROLLERS];
PADStatus s_prev_pad[PAD_MAX_CONTROLLERS];
u32 s_prev_pad_flag[PAD_MAX_CONTROLLERS];
PADMask s_pad_chan_mask[PAD_MAX_CONTROLLERS];

void init_mixed_controller() {
    s32 iController;

    PADInit();

    for (iController = 0; iController < PAD_MAX_CONTROLLERS; iController++) {
        s_pad[iController].button = 0;
        s_pad[iController].stickX = 0;
        s_pad[iController].stickY = 0;
        s_pad[iController].substickX = 0;
        s_pad[iController].substickY = 0;
        s_pad[iController].triggerLeft = 0;
        s_pad[iController].triggerRight = 0;
        s_pad[iController].analogA = 0;
        s_pad[iController].analogB = 0;
        s_pad[iController].err = 0;

        s_prev_pad_flag[iController] = 0;
    }

    s_pad_chan_mask[PAD_CHAN0] = PAD_CHAN0_BIT;
    s_pad_chan_mask[PAD_CHAN1] = PAD_CHAN1_BIT;
    s_pad_chan_mask[PAD_CHAN2] = PAD_CHAN2_BIT;
    s_pad_chan_mask[PAD_CHAN3] = PAD_CHAN3_BIT;
}
