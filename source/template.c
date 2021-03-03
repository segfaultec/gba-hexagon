

#include <gba_base.h>
#include <gba_video.h>
#include <gba_interrupt.h>		// for interrupt handling
#include <gba_systemcalls.h>	// for VBlankIntrWait()
#include <gba_types.h>
#include <gba_input.h>

#include <string.h>
#include <math.h>

#include "macros.h"
#include "input.h"
#include "lcd.h"

#include "font_img_bin.h"
#include "font_pal_bin.h"

#include "arrow_img_bin.h"
#include "arrow_pal_bin.h"

typedef struct __gbacolor {
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
	const u32 row_size = 0x400;

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

void* GetOAMPointer(u8 index) {
	return OAM + (sizeof(struct oam_regular) * index);
}

struct oam_affine_param* GetOAMAffinePointer(u8 index) {
	return (void*)OAM + (sizeof(struct oam_affine_param) * index);
}

void WriteToOAM(const void* data, u8 index) {
/* 
	Only the first 6 bytes of each entry to OAM is the object data.
	The last two bytes are reserved for the rotation/scaling params.

	This weirdness makes sure the last two bytes are untouched.
*/
	u32 upper = ((u32*)data)[0];
	u16 lower = ((u16*)data)[3];

	OAM[index*2] = upper;
	((u16*)OAM)[index*4 + 2] = lower;
}

void WriteToOAMUpper(const void* data, u8 index) {
/*
	Same as before but only writes the first 4 bytes.
	Most of the time we don't care about the index/palette,
	and this is way faster than that nonsense above.
*/

	OAM[index*2] = *((u32*)data);
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
	const struct oam_regular empty_sprite = {
		.disabled = true
	};

	// Clear OAM
	WriteToOAM(&empty_sprite, 0);
	CpuFastSet(OAM, OAM, FILL | COPY32 | (256));

	const struct oam_affine sprite_base = {
		.affine_enabled = true,
		.shape = OBJ_SHAPE_SQUARE,
		.size = OBJ_SIZE_16,
		.x = 100,
		.y = 100,
		.tile = 0,
		.affine_param_index = 0
	};

	WriteToOAM(&sprite_base, 0);

	u16* obj_tile_addr = (u16*)0x6010000;

	Copy8x8TileArea((u16*)arrow_img_bin, obj_tile_addr, 2, 2);
	//CpuFastSet(arrow_img_bin, obj_tile_addr, (arrow_img_bin_size/4) | COPY32);

	for (u8 i=0; i<arrow_pal_bin_size; i++) {
		((vu8*)OBJ_COLORS)[i] = arrow_pal_bin[i];
	} 
	

	struct oam_affine* main_sprite = GetOAMPointer(0);

	double counter = 0;

	struct oam_affine_param* main_affine = GetOAMAffinePointer(0);
	main_affine->pa =  main_affine->pd = 1<<8;
	main_affine->pb = main_affine->pc = 0;

	while (1) {
		// Code goes here!

		main_sprite->x = 100 + 20 * sin(counter);

		counter += 0.1;

		UpdateKeyDown();
		VBlankIntrWait();
	}
}