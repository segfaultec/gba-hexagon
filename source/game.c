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

static fixed32 angle = 0;
static unsigned int pattern_subindex = 0;

struct pattern_data pattern = {
	.index = 1, .active = true,
	.a = true, .b = true, .c = true, .d = true, .e = true, .f = true
};

struct pattern_data current_patterns[8];

void game_init() {
	//# BG Init
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
		{31, 31, 31}, // white (hex outer/player/pattern)
		{31, 31, 31} // alt white (pattern alt)
	};
	for (int i=0; i<(sizeof(main_pal)/sizeof(Color)); i++) {
		BG_PALETTE[i] = main_pal[i];
	}
	//#

	//# OAM init
	// Clear OAM
	const struct oam_regular empty_sprite = {
		.disabled = true
	};
	WriteToOAM(&empty_sprite, 0);
	CpuFastSet(OAM, OAM, FILL | COPY32 | 256);
	//#

	numdisplay_init(1, 30, 1);

	player_data_init();
	player_init(0);
}

void game_update() { 

	current_patterns[0] = pattern;

	numdisplay_update(0, pattern_subindex);

	pattern_draw_start(pattern_subindex);

	for (int i=0; i<8; i++) {
		pattern_draw(&current_patterns[i]);
	}

	pattern_draw_finish();

	struct calc_bg_rot_param bgparam ={
		.ptr = BGAFFINE2,
		.dx = 32840,
		.dy = 33800,
		.angle = angle,

		// as close as the camera can get
		// before showing the edge of the tilemap
		//.scale = fx_from_float(1.15),
		.scale = fx_from_float(1.0),

		.scr_x = fx_from_float(120),  
		.scr_y = fx_from_float(80),
	};

	CalcBGRotationMatrix(bgparam);

	player_update(angle, 0);

	//angle += 5;

	if (KEY_DOWN(Select)) angle = 0;
	
	if (angle > pi) {
		angle -= tau;
	}
	if (angle < -pi) {
		angle += tau;
	}
}