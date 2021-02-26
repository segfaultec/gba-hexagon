

#include <gba_base.h>
#include <gba_video.h>
#include <gba_interrupt.h>		// for interrupt handling
#include <gba_systemcalls.h>	// for VBlankIntrWait()
#include <gba_types.h>
#include <gba_input.h>

#include "string.h"

#include "macros.h"
#include "input.h"
#include "lcd.h"

#include "font_img_bin.h"
#include "font_pal_bin.h"

#include "arrow_img_bin.h"
#include "arrow_pal_bin.h"

typedef struct __gbacolor {
	unsigned int r : 5;
	unsigned int b : 5;
	unsigned int g : 5;
	bool _unused : 1;
} PK_AL(2) Color;

u8 Lerp8(u8 A, u8 B, u8 F) {
    if (F == 0) return A;
    if (F == 0xFF) return B;
    F++;
    return A + (u8)(((u16)(B-A) * F) >> 8);
}

Color mixcolor(Color a, Color b, u8 mix) {
	a.r = Lerp8(a.r, b.r, mix);
	a.g = Lerp8(a.g, b.g, mix);
	a.b = Lerp8(a.b, b.b, mix);
	return a;
}

const u8 hello_world_indexes[] = {1,2,3,3,4,9,5,4,6,3,7,8};

int main(void) {

	irqInit();
	irqEnable(IRQ_VBLANK);

	struct dispcnt* dispcnt = (struct dispcnt*)0x4000000;
	*dispcnt = dispcnt_zero;
	dispcnt->bg_mode = 0;
	dispcnt->bg0_on = true;
	dispcnt->obj_on = true;

	for (u8 i=0; i<font_pal_bin_size; i++) {
		((u8*)BG_COLORS)[i] = font_pal_bin[i];
	}


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

	while (1) {
		// Code goes here!

		UpdateKeyDown();
		VBlankIntrWait();
	}
}