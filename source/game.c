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

fixed32 x; fixed32 y;

void game_init() {
	*DISCNT = dispcnt_zero;
	DISCNT->bg_mode = 1;
	DISCNT->bg2_on = true;
	DISCNT->obj_on = false;

	BGCNT2->char_base_16k_block = 0;
	BGCNT2->screen_base_2k_block = 31;
	BGCNT2->screen_size = 1;

	CpuFastSet(hexagon_reduced_img_bin, VRAM_BASE, COPY32 | (hexagon_reduced_img_bin_size / 4));

	struct bgmap* map = (void*)0x600f800;

	*(u32*)map = 0;
	CpuFastSet(map, map, FILL | COPY32 | (0x800/4));

	CpuFastSet(hexagon_reduced_pal_bin, BG_PALETTE, COPY32 | (hexagon_reduced_pal_bin_size/4));

	CpuFastSet(hexagon_map_bin, map, COPY32 | (hexagon_map_bin_size/4));

	// const struct oam_regular empty_sprite = {
	// 	.disabled = true,
	// };
	// WriteToOAM(&empty_sprite, 0);
	// CpuFastSet(OAM, OAM, FILL | COPY32 | 256);

	// const struct oam_regular center_dot_sprite = {
	// 	.x
	// }

	x = fx_from_int(134);
	y = fx_from_int(130);
}

fixed32 angle = 0;

const fixed32 movespeed = 1 << 8;

void game_update() { 
	

	CalcBGRotationMatrix(BGAFFINE2, x, y, angle, fx_from_float(1.5));

	if (KEY_HELD(Left)) x-=movespeed;
	if (KEY_HELD(Right)) x+=movespeed;
	if (KEY_HELD(Up)) y-=movespeed;
	if (KEY_HELD(Down)) y+=movespeed;
	if (KEY_HELD(B)) angle -= fx_from_float(0.05);
	if (KEY_HELD(A)) angle += fx_from_float(0.05);
	
	if (angle > pi) {
		angle -= tau;
	}
	if (angle < -pi) {
		angle += tau;
	}
}