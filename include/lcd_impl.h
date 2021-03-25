#pragma once

#include "types.h"
#include "lcd_types.h"

void CalcOAMRotationMatrix(volatile struct oam_affine_param* ptr, fixed32 angle, fixed32 scale);

void CalcBGRotationMatrix(volatile struct bg_affine_param* ptr, fixed32 dx, fixed32 dy, fixed32 angle, fixed32 scale);

void Copy8x8TileArea(void* source, void* dest, u32 width, u32 height);

volatile void* GetOAMPointer(u8 index);
volatile struct oam_affine_param* GetOAMAffinePointer(u8 index);

void WriteToOAM(const void* data, u8 index);
void WriteToOAMUpper(const void* data, u8 index);