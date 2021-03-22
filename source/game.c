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


void game_init() {
    struct dispcnt* dispcnt = (struct dispcnt*)0x4000000;
	*dispcnt = dispcnt_zero;
	dispcnt->bg_mode = 4;
	dispcnt->bg2_on = true;
	dispcnt->obj_on = false;
	
	const Color black = {0, 0, 0};
	const Color red = {31, 0, 0};
	const Color white = {31, 31, 31};
	const Color blue = {0, 0, 31};

	Color* const palette = (Color*)0x05000000;
	palette[0] = black;
	palette[1] = red;
	palette[2] = white;
	palette[3] = blue;

	u32* const frame0 = (u32*)0x6000000;
	u32* const frame1 = (u32*)0x600A000;

	const u32 frame_size = 0xA000;

	const u8 pixels_f0[4] = {0, 0, 2, 2};
	frame0[0] = *(u32*)pixels_f0;

	CpuFastSet(frame0, frame0, FILL | COPY32 | frame_size / 4 );

	const u8 pixels_f1[4] = {0, 0, 3, 3};
	frame1[0] = *(u32*)pixels_f1;

	CpuFastSet(frame1, frame1, FILL | COPY32 | frame_size / 4 );
}

void init_bg() {

}


void game_update() { 
	struct dispcnt* dispcnt = (struct dispcnt*)0x4000000;
	//dispcnt->display_frame_select = !dispcnt->display_frame_select;
}