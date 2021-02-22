

#include <gba_base.h>
#include <gba_video.h>
#include <gba_interrupt.h>		// for interrupt handling
#include <gba_systemcalls.h>	// for VBlankIntrWait()
#include <gba_types.h>
#include <gba_input.h>

#include "string.h"

#include "macros.h"
#include "input.h"

#include "font_img_bin.h"
#include "font_pal_bin.h"

typedef struct __gbacolor {
	unsigned int r : 5;
	unsigned int b : 5;
	unsigned int g : 5;
	bool _unused : 1;
} PK_AL(2) Color;

// void setcolor(Color color) {
// 	u16* BGPALETTE = (u16*)0x5000000;
// 	BGPALETTE[0] = REINT_CAST(u16, color);
// }

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

static u8 bg_gradient = 0;
static s8 bg_grad_dir = 1;
static s8 bg_grad_speed = 5;

static u32 hblank_wait = 0;

void irq_hblank()
{
	const Color red = {20,30,0};
	const Color blue = {25,17,5};
	
	Color* bgpalette = (Color*)BG_COLORS;

	bgpalette[0] = mixcolor(red, blue, bg_gradient);

	u8 newgradient = bg_gradient;
	if (bg_grad_dir > 0) {
		newgradient += bg_grad_speed;
		if (newgradient < bg_gradient) {
			bg_grad_dir *= -1;
		} else {
			bg_gradient = newgradient;
		}
	} else {
		newgradient -= bg_grad_speed;
		if (newgradient > bg_gradient) {
			bg_grad_dir *= -1;
		} else {
			bg_gradient = newgradient;
		}
	}

	for (int i=0; i<hblank_wait; i++) {
		hblank_wait++;
		hblank_wait--;
	}

	REG_IF = IRQ_HBLANK;
}

int main(void) {

	irqInit();
	irqEnable(IRQ_VBLANK | IRQ_HBLANK);

	irqSet(IRQ_HBLANK, irq_hblank);

	struct16z(dispcnt_data,
		/* 0-2 */ unsigned int bg_mode : 3;
		/* 3   */ char __reserved1 : 1;
		/* 4   */ unsigned int display_frame_select : 1;
		/* 5   */ bool hblank_interval_free : 1;
		/* 6   */ bool linear_obj_char_vram_mapping : 1;
		/* 7   */ bool fblank : 1;
		/* 8   */ bool bg0_on : 1;
		/* 9   */ bool bg1_on : 1;
		/* 10  */ bool bg2_on : 1;
		/* 11  */ bool bg3_on : 1;
		/* 12  */ bool obj_on : 1;
		/* 13  */ bool win0_display_flag : 1;
		/* 14  */ bool win1_display_flag : 1;
		/* 15  */ bool winobj_display_flag : 1;
	);

	struct dispcnt_data* dispctrl = (struct dispcnt_data*)0x4000000;
	*dispctrl = dispcnt_data_zero;
	dispctrl->bg_mode = 0;
	dispctrl->bg0_on = true;

	for (u8 i=0; i<font_pal_bin_size; i++) {
		((u8*)BG_COLORS)[i] = font_pal_bin[i];
	}

	struct16z(bgcnt_data,
		/* 0-1 */ unsigned int priority : 2;
		/* 2-3 */ unsigned int char_base_16k_block : 2;
		/* 4-5 */ char __reserved1 : 2;
		/* 6   */ bool mosaic : 1;
		/* 7   */ bool palette_256 : 1;
		/* 8-12*/ unsigned int screen_base_2k_block : 5;
		/* 13  */ char __reserved2 : 1;
		/*14-15*/ unsigned int screen_size : 2;
	);

	struct bgcnt_data* const bgctrl = (struct bgcnt_data*)BGCTRL;

	*bgctrl = bgcnt_data_zero;
	bgctrl->char_base_16k_block = 0;
	bgctrl->screen_base_2k_block = 31;

	struct bgmap_data* map = MAP_BASE_ADR(31);

	*(u32 *)MAP_BASE_ADR(31) = 0;
	CpuFastSet(MAP_BASE_ADR(0), MAP_BASE_ADR(0), FILL | COPY32 | (0x800/4));

	struct16(bgmap_data,
		/* 0-9 */ unsigned int tile : 10;
		/* 10  */ bool flipH : 1;
		/* 11  */ bool flipV : 1;
		/*12-15*/ unsigned int palette : 4;
	);

	for (u8 i=0; i<sizeof(hello_world_indexes); i++) {
		map[i].tile = hello_world_indexes[i];
	}
	
	CpuFastSet(font_img_bin, (u16*)VRAM,(font_img_bin_size/4) | COPY32);

	while (1) {
		if (KEY_HELD(Down)) {
			bg_grad_speed += 1;
		}
		if (KEY_HELD(Up)) {
			bg_grad_speed -= 1;
		}
		if (KEY_HELD(Right)) {
			hblank_wait += 1;
		}
		if (KEY_HELD(Left)) {
			if (hblank_wait > 1)
				hblank_wait -= 1;
			else
				hblank_wait = 0;
		}

		UpdateKeyDown();
		VBlankIntrWait();

		bg_gradient = 0;
	}

}

