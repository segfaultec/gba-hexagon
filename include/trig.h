#pragma once

#include "types.h"
#include "macros.h"

static const fixed32 pi = 804;
static const fixed32 tau = 1608;

extern const fixed32 sin_lut[1024];

fixed32 my_sine(fixed32 angle);
fixed32 my_cosine(fixed32 angle);