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

	CpuFastSet(hexagon_reduced_img_bin, VRAM_BASE, COPY32 | (hexagon_reduced_img_bin_size / 4));

	struct bgmap* map = (void*)0x600f800;

	*(u32*)map = 0;
	CpuFastSet(map, map, FILL | COPY32 | (0x800/4));

	CpuFastSet(hexagon_reduced_pal_bin, (void*)BG_PALETTE, COPY32 | (hexagon_reduced_pal_bin_size/4));

	const Color black = {0, 0, 0};
	const Color cyan  = {0, 31, 31};
	BG_PALETTE[1] = black;
	BG_PALETTE[2] = cyan;

	CpuFastSet(hexagon_map_bin, map, COPY32 | (hexagon_map_bin_size/4));

	const struct oam_regular empty_sprite = {
		.disabled = true
	};

	WriteToOAM(&empty_sprite, 0);
	CpuFastSet(OAM, OAM, FILL | COPY32 | 256);

	x = 34830;
	y = 33900;

	numdisplay_init(1, 5, 1);

	player_data_init();
	player_init(0);
}

static fixed32 angle = 0;

void game_update() { 
	struct calc_bg_rot_param bgparam ={
		.ptr = BGAFFINE2,
		.dx = x,
		.dy = y,
		.angle = angle,
		.scale = fx_from_float(1.2),
		.scr_x = fx_from_float(118),
		.scr_y = fx_from_float(80),
	};

	CalcBGRotationMatrix(bgparam);

	player_update(angle, 0);

	angle += 10;

	if (KEY_DOWN(Select)) angle = 0;
	
	if (angle > pi) {
		angle -= tau;
	}
	if (angle < -pi) {
		angle += tau;
	}
}