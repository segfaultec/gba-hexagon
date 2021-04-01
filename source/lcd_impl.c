#include "lcd_impl.h"

#include "trig.h"
#include <gba_systemcalls.h>

void CalcOAMRotationMatrix(volatile struct oam_affine_param* ptr, fixed32 angle, fixed32 scale) {
	// [ pa  pb ]
	// [ pc  pd ]

	fixed32 calc_sin = my_sine(angle);
	fixed32 calc_cos = my_cosine(angle);

	ptr->pa = fx_division(calc_cos, scale);
	ptr->pb = fx_division(-calc_sin, scale);
	ptr->pc = fx_division(calc_sin, scale);
	ptr->pd = fx_division(calc_cos, scale);
}

void CalcBGRotationMatrix(struct calc_bg_rot_param param) {
	fixed32 calc_sin = my_sine(param.angle);
	fixed32 calc_cos = my_cosine(param.angle);

	fixed32 pa = fx_division(calc_cos, param.scale);
	fixed32 pb = fx_division(-calc_sin, param.scale);
	fixed32 pc = fx_division(calc_sin, param.scale);
	fixed32 pd = fx_division(calc_cos, param.scale);
	// 120 80
	fixed32 scr_x = param.scr_x;
	fixed32 scr_y = param.scr_y;

	BGAFFINE2->dx = param.dx - (fx_multiply(scr_x, pa) + fx_multiply(scr_y, pb));
	BGAFFINE2->dy = param.dy - (fx_multiply(scr_x, pc) + fx_multiply(scr_y, pd));

	param.ptr->pa = pa;
	param.ptr->pb = pb;
	param.ptr->pc = pc;
	param.ptr->pd = pd;
}

void Copy8x8TileArea(void* source, void* dest, u32 width, u32 height) {
	const u32 tile_word_size = 0x8;
	const u32 row_size = 1024;

	while (height-- > 0) {
		CpuFastSet(
			source, 
			dest,
			COPY32 | (tile_word_size * width)
		);
		source = source + (tile_word_size * width * 4);
		dest += row_size;
	}
}

volatile void* GetOAMPointer(u8 index) {
	return OAM + (sizeof(struct oam_regular) * index);
}

volatile struct oam_affine_param* GetOAMAffinePointer(u8 index) {
	return (void*)OAM + (sizeof(struct oam_affine_param) * index);
}

void WriteToOAM(const void* data, u8 index) {
/* 
	Only the first 6 bytes of each entry to OAM is the object data.
	The last two bytes are reserved for the rotation/scaling params.

	This weirdness makes sure the last two bytes are untouched.
*/

	// This masks off the oam attribute quarter.
	// It's backwards because of little-endian.
	const u32 lower_mask = 0x0000FFFF;
	u32* source = (u32*)data;
	u32* dest = (u32*)OAM;

	u32 upper = source[0];
	u32 lower = source[1];

	// Write to upper byte, as normal
	dest[index*2] = upper;

	lower &= lower_mask;
	lower |= (dest[index*2+1] & (~lower_mask));

	dest[index*2+1] = lower;
}

void WriteToOAMUpper(const void* data, u8 index) {
/*
	Same as before but only writes the first 4 bytes.
	Most of the time we don't care about the index/palette,
	and this is way faster than that nonsense above.
*/

	OAM[index*2] = *((u32*)data);
}