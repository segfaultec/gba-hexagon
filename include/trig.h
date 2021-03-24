#pragma once

#include "types.h"
#include "macros.h"

extern const fixed32 pi;
extern const fixed32 tau;

extern const fixed32 sin_lut[1024];

fixed32 my_sine(fixed32 angle);
fixed32 my_cosine(fixed32 angle);