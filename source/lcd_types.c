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

struct dispcnt* const DISCNT = (struct dispcnt*)0x4000000;
Color* const BG_PALETTE = (Color*)0x05000000;
u32* const BG_BMP_FRAME0 = (u32*)0x6000000;
u32* const BG_BMP_FRAME1 = (u32*)0x600A000;
const u32 BG_BMP_FRAME_SIZE = 0xA000;