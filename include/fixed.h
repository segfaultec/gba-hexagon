#pragma once

#include <gba_types.h>

#include "macros.h"

// GBA 8.8 Fixed point decimal format

// Used https://www.coranac.com/tonc/text/fixed.htm#sec-fmath
// to help with conversion functions.

typedef s32 fixed32;
typedef s16 fixed16;

#define FIX_SHIFT 8
#define FIX_SCALE 256
#define FIX_MASK 255
#define FIX_SCALEF 256.0f

INLINE fixed32 fx_from_int(int d) {
    return d << FIX_SHIFT;
}

INLINE fixed32 fx_from_float(float f) {
    return (fixed32)(f*FIX_SCALE);
}

INLINE u32 fx_integer_part(fixed32 fx) {
    return fx >> FIX_SHIFT;
}

INLINE u32 fx_fractional_part(fixed32 fx) {
    return fx & FIX_MASK;
}

INLINE int fx_to_int(fixed32 fx) {
    return fx / FIX_SCALE;
}

INLINE float fx_to_float(fixed32 fx) {
    return fx / FIX_SCALEF;
}

INLINE fixed32 fx_multiply(fixed32 fa, fixed32 fb) {
    return (fa*fb)>>FIX_SHIFT;
}

INLINE fixed32 fx_division(fixed32 fa, fixed32 fb) {
    return ((fa)*FIX_SCALE)/(fb);
}