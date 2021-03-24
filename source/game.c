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
	BG_PALETTE[255] = white;

	//BG_BMP_FRAME0[0] = 0x01020300;
}

Point hexcalc(const Point center, const fixed32 angle, const u32 step, const u32 multi) {
	fixed32 pi_div_3 = fx_division(pi, fx_from_int(3));
	fixed32 step_fx = fx_multiply(fx_from_int(step), pi_div_3);

	fixed32 x1 = my_cosine(step_fx);
	fixed32 y1 = my_sine(step_fx);

	fixed32 s = my_sine(angle);
	fixed32 c = my_cosine(angle);

	fixed32 x2 = fx_multiply(x1, c) - fx_multiply(y1, s);
	fixed32 y2 = fx_multiply(x1, s) + fx_multiply(y1, c);

	Point point = {
		center.x + fx_integer_part(fx_multiply(x2, fx_from_int(multi))),
		center.y + fx_integer_part(fx_multiply(y2, fx_from_int(multi)))
	};

	return point;
}

INLINE u32* CoordToAddr(void* base, Point point) {
	const u32 line_size = 240;
	//const u32 line_count = 160;

	void* ptr = base;
	ptr += line_size * point.y;
	ptr += point.x;

	return ptr;
}

void WriteToBMP(void* ptr, u32 value) {
	u32 mod = (u32)ptr % 4;
	u32 toWrite = value << (mod * 8);
	// switch (mod)
	// {
	// 	case 0:
	// 		toWrite = 0x000000FF;
	// 		break;
	// 	case 1:
	// 		toWrite = 0x0000FF00;
	// 		break;
	// 	case 2:
	// 		toWrite = 0x00FF0000;
	// 		break;
	// 	case 3:
	// 		toWrite = 0xFF000000;
	// 		break;
		
	// 	default:
	// 		Halt();
	// 		break;
	// }
	ptr = ptr - mod;
	*(u32*)ptr |= toWrite;
}

fixed32 angle = 0;

void game_update() { 
	
	BG_BMP_FRAME0[0] = 0;
	CpuFastSet(BG_BMP_FRAME0, BG_BMP_FRAME0, FILL | COPY32 | 0x2800);

	const Point center = {100, 100};

	for (u32 i=0; i<6; i++) {
		Point point = hexcalc(center, angle, i, 20);
		WriteToBMP(CoordToAddr(BG_BMP_FRAME0, point), 2);
	}
	
	angle += 1;

	if (angle > tau) {
		angle -= pi;
	}

	//DISCNT->display_frame_select = !DISCNT->display_frame_select;
}