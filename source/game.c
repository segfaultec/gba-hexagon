#include "font_img_bin.h"
#include "font_pal_bin.h"
#include "arrow_img_bin.h"
#include "arrow_pal_bin.h"

#include <gba_base.h>
#include <gba_video.h>
#include <gba_types.h>
#include <gba_input.h>
#include <gba_systemcalls.h>

#include "game.h"
#include "fixed.h"
#include "lcd_types.h"
#include "lcd_impl.h"
#include "color.h"
#include "trig.h"

static const u8 hello_world_indexes[] = {1,2,3,3,4,9,5,4,6,3,7,8};
static fixed32 counter;

static volatile struct oam_affine* m_main_sprite;
static volatile struct oam_affine_param* m_main_affine;

void game_init() {
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

	m_main_sprite = GetOAMPointer(0);

	counter = 0;

	m_main_affine = GetOAMAffinePointer(0);
}

void init_bg() {

}


void game_update() {
    // Code goes here!

    fixed32 sin_counter = my_sine(counter);

    CalcRotationMatrix(
        m_main_affine,
        counter,
        fx_division(sin_counter, fx_from_int(2)) + fx_from_float(1)
    );

    //main_sprite->x = 100;
    m_main_sprite->x = fx_to_int(
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
}