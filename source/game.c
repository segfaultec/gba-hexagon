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
#include "input.h"

#include "numdisplay.h"
#include "player.h"
#include "patterns.h"

const u32 player_count = 32;
fixed32 x; fixed32 y;

void game_init() {
	*DISCNT = dispcnt_zero;
	DISCNT->bg_mode = 1;
	DISCNT->bg0_on = true;
	DISCNT->bg2_on = true;
	DISCNT->obj_on = true;

	BGCNT2->char_base_16k_block = 0;
	BGCNT2->screen_base_2k_block = 31;
	BGCNT2->screen_size = 1;

	// Clear map
	struct bgmap* map = (void*)(0x6000000 + (31<<11));
	*(u32*)map = 0;
	CpuFastSet(map, map, FILL | COPY32 | (0x800>>2));

	// Set BG pallete
	const Color main_pal[] = {
		{0, 0, 0}, // black (hex center)
		{0, 0, 0}, // col 1
		{0, 31, 31}, // col 2
		{26, 26, 26}, // gray (hex inner)
		{31, 31, 31} // white (hex outer/player)
	};
	for (int i=0; i<(sizeof(main_pal)/sizeof(Color)); i++) {
		BG_PALETTE[i] = main_pal[i];
	}

	// Clear OAM
	const struct oam_regular empty_sprite = {
		.disabled = true
	};
	WriteToOAM(&empty_sprite, 0);
	CpuFastSet(OAM, OAM, FILL | COPY32 | 256);

	x = 32840;
	y = 33800;

	patterns_init();

	numdisplay_init(1, 30, 1);

	player_data_init();
	player_init(0);
}

static fixed32 angle = 0;

void game_update() { 
	struct bgmap* map = (void*)(0x6000000 + (31<<11));
	CpuFastSet(hexagon_map_bin, map, COPY32 | (hexagon_map_bin_size>>2));

	patterns_update();

	struct calc_bg_rot_param bgparam ={
		.ptr = BGAFFINE2,
		.dx = x,
		.dy = y,
		.angle = angle,
		.scale = fx_from_float(1.2),
		.scr_x = fx_from_float(120),
		.scr_y = fx_from_float(80),
	};

	CalcBGRotationMatrix(bgparam);

	player_update(angle, 0);

	//angle += 10;

	if (KEY_DOWN(Select)) angle = 0;
	
	if (angle > pi) {
		angle -= tau;
	}
	if (angle < -pi) {
		angle += tau;
	}
}