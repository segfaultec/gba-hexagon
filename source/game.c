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
	BG_PALETTE[1] = red;
	BG_PALETTE[2] = white;
	BG_PALETTE[3] = blue;

	BG_BMP_FRAME0[0] = 0x01020300;
}

void init_bg() {

}


void game_update() { 
	

	//DISCNT->display_frame_select = !DISCNT->display_frame_select;
}