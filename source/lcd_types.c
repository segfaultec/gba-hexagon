#include "lcd_types.h"

const struct dispcnt dispcnt_zero = {};

const struct bgcnt bgcnt_zero = {};

const struct oam_regular oam_regular_zero = { 
    .affine_enabled = 0
};

const struct oam_affine oam_affine_zero = {
    .affine_enabled = 1,
};

u32* const OAM = (u32*)0x07000000;