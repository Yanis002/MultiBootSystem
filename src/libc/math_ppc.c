#include "macros.h"
#include "fdlibm.h"

WEAK float acosf(float x) {
    return acos(x);
}

WEAK float cosf(float x) {
    return cos(x);
}

WEAK float sinf(float x) {
    return sin(x);
}

WEAK float tanf(float x) {
    return tan(x);
}
