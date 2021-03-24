#include "hexagon_reduced_img_bin.h"
#include "hexagon_reduced_pal_bin.h"
#include "hexagon_map_bin.h"

#include <gba_systemcalls.h>

#include "game.h"
#include "types.h"
#include "lcd_types.h"
#include "lcd_impl.h"
#include "color.h"
#include "trig.h"

void game_init() {
	*DISCNT = dispcnt_zero;
	DISCNT->bg_mode = 0;
	DISCNT->bg0_on = true;
	DISCNT->obj_on = false;

	BGCNT0->char_base_16k_block = 0;
	BGCNT0->screen_base_2k_block = 31;

	CpuFastSet(hexagon_reduced_img_bin, VRAM_BASE, COPY32 | (hexagon_reduced_img_bin_size / 4));

	struct bgmap* map = (void*)0x600f800;
	*(u32*)map = 0;
	CpuFastSet(map, map, FILL | COPY32 | (0x800/4));

	CpuFastSet(hexagon_map_bin, map, COPY32 | (hexagon_map_bin_size/4));
	CpuFastSet(hexagon_reduced_pal_bin, BG_PALETTE, COPY32 | (hexagon_reduced_pal_bin_size/4));
}


void game_update() { 
	
}