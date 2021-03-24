#include "font_img_bin.h"
#include "font_pal_bin.h"
#include "arrow_img_bin.h"
#include "arrow_pal_bin.h"

#include <gba_systemcalls.h>

#include "game.h"
#include "types.h"
#include "lcd_types.h"
#include "lcd_impl.h"
#include "color.h"
#include "trig.h"


void game_init() {
	*DISCNT = dispcnt_zero;
	DISCNT->bg_mode = 4;
	DISCNT->bg2_on = true;
	DISCNT->obj_on = false;
	
	const Color black = {0, 0, 0};
	const Color red = {31, 0, 0};
	const Color white = {31, 31, 31};
	const Color blue = {0, 0, 31};

	BG_PALETTE[0] = black;
	BG_PALETTE[1] = white;
	BG_PALETTE[2] = red;
	BG_PALETTE[3] = blue;

	BG_BMP_FRAME0[0] = 0x01020300;
}

Point hexcalc(const u32 center_x, const u32 center_y, const fixed32 angle, const u32 step, const u32 multi) {
	fixed32 pi_div_3 = fx_division(pi, fx_from_int(3));
	fixed32 step_fx = fx_multiply(fx_from_int(step), pi_div_3);

	fixed32 x1 = fx_multiply(my_cosine(step_fx), fx_from_int(multi));
	fixed32 y1 = fx_multiply(my_sine(step_fx), fx_from_int(multi));

	Point point = {
		center_x + fx_to_int(x1),
		center_y + fx_to_int(y1)
	};

	return point;
}

// INLINE u32* coord_to_addr(u32* base, Point point) {
// 	const 
// }

void game_update() { 
	

	//DISCNT->display_frame_select = !DISCNT->display_frame_select;
}