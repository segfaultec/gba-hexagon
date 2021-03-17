#pragma once

#include "fixed.h"
#include "macros.h"

const fixed32 pi = 804;
const fixed32 tau = 1608;

extern const fixed32 sin_lut[1024];

INLINE fixed32 my_sine(fixed32 angle) {
    u32 lookup = (angle * 512 / pi) & 0x3ff;
    return sin_lut[lookup];
}

INLINE fixed32 my_cosine(fixed32 angle) {
    return my_sine(angle + (pi / 2));
}