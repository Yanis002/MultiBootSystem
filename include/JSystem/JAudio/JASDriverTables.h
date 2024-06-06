#ifndef JASDRIVERTABLES_H
#define JASDRIVERTABLES_H

#include "dolphin/types.h"
#include "macros.h"

namespace JASystem {
    namespace Driver {
        extern f32 C5BASE_PITCHTABLE[];
    }
    namespace DSPInterface {
        extern u16 DSPADPCM_FILTER[] ATTRIBUTE_ALIGN(32);
        extern u16 DSPRES_FILTER[] ATTRIBUTE_ALIGN(32);
    } // namespace DSPInterface
} // namespace JASystem

#endif /* JASDRIVERTABLES_H */
