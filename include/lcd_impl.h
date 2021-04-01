#pragma once

#include "types.h"
#include "lcd_types.h"

void CalcOAMRotationMatrix(volatile struct oam_affine_param* ptr, fixed32 angle, fixed32 scale);

struct calc_bg_rot_param {
	volatile struct bg_affine_param* ptr;
	fixed32 dx; fixed32 dy;
	fixed32 scr_x; fixed32 scr_y;
	fixed32 angle; fixed32 scale;
};

void CalcBGRotationMatrix(struct calc_bg_rot_param param);

void Copy8x8TileArea(void* source, void* dest, u32 width, u32 height);

volatile void* GetOAMPointer(u8 index);
volatile struct oam_affine_param* GetOAMAffinePointer(u8 index);

void WriteToOAM(const void* data, u8 index);
void WriteToOAMUpper(const void* data, u8 index);