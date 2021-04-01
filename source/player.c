#include "player.h"

#include "types.h"
#include "input.h"
#include "trig.h"
#include "numdisplay.h"

#include "lcd_types.h"
#include "lcd_impl.h"

#include "triangle_img_bin.h"

static fixed32 angle = 0;

const static fixed32 movespeed = 30;

static struct oam_regular player_sprite = {
    .shape = OBJ_SHAPE_SQUARE,
    .size = OBJ_SIZE_8,
    .x = 20,
    .y = 20,
    .tile = 0,
    .pallette = 0
};

void player_data_init() {
    OAM_PALETTE[1] = white;

    u16* obj_tile_addr = (u16*)0x6010000;

    Copy8x8TileArea((u16*)triangle_img_bin, obj_tile_addr, 1, 1);
}

void player_init(u32 index) {
    WriteToOAM(&player_sprite, index);
}

void player_update(fixed32 angleoffset, u32 index) {
    if (KEY_HELD(Left)) angle -= movespeed;
    if (KEY_HELD(Right)) angle += movespeed;

    fixed32 sin_calc = my_sine(angle - angleoffset);
    fixed32 cos_calc = my_cosine(angle - angleoffset);

    fixed32 x = cos_calc * (25) - sin_calc * (0);
    fixed32 y = sin_calc * (25) + cos_calc * (0);

    player_sprite.x = fx_to_int(x) + 115;
    player_sprite.y = fx_to_int(y) + 75;

    if (angle > pi) {
		angle -= tau;
	}
	if (angle < -pi) {
		angle += tau;
	}

    //numdisplay_update(0, angle);
    //numdisplay_update(1, angleoffset);
    WriteToOAM(&player_sprite, index);
}