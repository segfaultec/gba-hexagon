

#include <gba_base.h>
#include <gba_video.h>
#include <gba_interrupt.h>		// for interrupt handling
#include <gba_systemcalls.h>	// for VBlankIntrWait()
#include <gba_types.h>
#include <gba_input.h>

#include <string.h>

#include "trig.h"
#include "macros.h"
#include "input.h"
#include "lcd.h"
#include "fixed.h"

#include "font_img_bin.h"
#include "font_pal_bin.h"

#include "arrow_img_bin.h"
#include "arrow_pal_bin.h"

typedef struct Color {
	unsigned int r : 5;
	unsigned int b : 5;
	unsigned int g : 5;
	bool _unused : 1;
} PK_AL(2) Color;

u8 Lerp8(u8 A, u8 B, u8 F) {
    if (F == 0) return A;
    if (F == 0xFF) return B;
    F++;
    return A + (u8)(((u16)(B-A) * F) >> 8);
}

Color mixcolor(Color a, Color b, u8 mix) {
	a.r = Lerp8(a.r, b.r, mix);
	a.g = Lerp8(a.g, b.g, mix);
	a.b = Lerp8(a.b, b.b, mix);
	return a;
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

void CalcRotationMatrix(volatile struct oam_affine_param* ptr, fixed32 angle, fixed32 scale) {
	// [ pa  pb ]
	// [ pc  pd ]

	fixed32 calc_sin = my_sine(angle);
	fixed32 calc_cos = my_cosine(angle);

	ptr->pa = fx_division(calc_cos, scale);
	ptr->pb = fx_division(-calc_sin, scale);
	ptr->pc = fx_division(calc_sin, scale);
	ptr->pd = fx_division(calc_cos, scale);
}

const u8 hello_world_indexes[] = {1,2,3,3,4,9,5,4,6,3,7,8};

int main(void) {
//#
	irqInit();
	irqEnable(IRQ_VBLANK);
	
	struct dispcnt* dispcnt = (struct dispcnt*)0x4000000;
	*dispcnt = dispcnt_zero;
	dispcnt->bg_mode = 0;
	dispcnt->bg0_on = true;
	dispcnt->obj_on = true;

	for (u8 i=0; i<font_pal_bin_size; i++) {
		((u8*)BG_COLORS)[i] = font_pal_bin[i];
	}

	const Color red = {12,0,0};
	((Color*)BG_COLORS)[0] = red;

	struct bgcnt* const bgcnt = (struct bgcnt*)BGCTRL;

	*bgcnt = bgcnt_zero;
	bgcnt->char_base_16k_block = 0;
	bgcnt->screen_base_2k_block = 31;

	struct bgmap* map = MAP_BASE_ADR(31);

	// Clear Map
	*(u32 *)MAP_BASE_ADR(31) = 0;
	CpuFastSet(MAP_BASE_ADR(31), MAP_BASE_ADR(31), FILL | COPY32 | (0x800/4));

	for (u8 i=0; i<sizeof(hello_world_indexes); i++) {
		map[i].tile = hello_world_indexes[i];
	} 
	
	CpuFastSet(font_img_bin, (u16*)VRAM,(font_img_bin_size/4) | COPY32);
//#

//#
	const struct oam_regular empty_sprite = {
		.disabled = true,
	};

	// Clear OAM
	WriteToOAM(&empty_sprite, 0);
	CpuFastSet(OAM, OAM, FILL | COPY32 | (256));

	const struct oam_affine sprite_base = {
		.affine_enabled = true,
		.double_size_enabled = true,
		.shape = OBJ_SHAPE_SQUARE,
		.size = OBJ_SIZE_16,
		.x = 100,
		.y = 100,
		.tile = 0,
	};

	WriteToOAM(&sprite_base, 0);

	void* obj_tile_addr = (void*)0x6010000;

	Copy8x8TileArea((void*)arrow_img_bin, obj_tile_addr, 2, 2);
	//CpuFastSet(arrow_img_bin, obj_tile_addr, (arrow_img_bin_size/4) | COPY32);

	for (u8 i=0; i<arrow_pal_bin_size; i++) {
		((vu8*)OBJ_COLORS)[i] = arrow_pal_bin[i];
	} 

//#

	volatile struct oam_affine* main_sprite = GetOAMPointer(0);

	fixed32 counter = 0;

	volatile struct oam_affine_param* main_affine = GetOAMAffinePointer(0);

	while (1) {
		// Code goes here!

		fixed32 sin_counter = my_sine(counter);

		CalcRotationMatrix(
			main_affine,
			counter,
			fx_division(sin_counter, fx_from_int(2)) + fx_from_float(1)
		);

		//main_sprite->x = 100;
		main_sprite->x = fx_to_int(
			fx_from_int(100) + 
			fx_multiply(
				fx_from_int(20),
				sin_counter
			)
		);

		counter += fx_from_float(0.1);
		// if (counter > M_PI * 4) {
		// 	counter -= M_PI * 2;
		// }

		UpdateKeyDown();
		VBlankIntrWait();
	}
}