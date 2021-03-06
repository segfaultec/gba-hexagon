#pragma once

#include "macros.h"
#include "fixed.h"

//# Display Control

struct dispcnt {
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
} PK_AL(2);

const struct dispcnt dispcnt_zero = {};

struct bgcnt {
    /* 0-1 */ unsigned int priority : 2;
    /* 2-3 */ unsigned int char_base_16k_block : 2;
    /* 4-5 */ char __reserved1 : 2;
    /* 6   */ bool mosaic : 1;
    /* 7   */ bool palette_256 : 1;
    /* 8-12*/ unsigned int screen_base_2k_block : 5;
    /* 13  */ char __reserved2 : 1;
    /*14-15*/ unsigned int screen_size : 2;
} PK_AL(2);

const struct bgcnt bgcnt_zero = {};

//#

//# Map Data

struct bgmap {
    /* 0-9 */ unsigned int tile : 10;
    /* 10  */ bool flipH : 1;
    /* 11  */ bool flipV : 1;
    /*12-15*/ unsigned int palette : 4;
} PK_AL(2);

//#

//# LCD OBJ 

enum OBJ_MODE {
    OBJ_MODE_NORMAL = 0,
    OBJ_MODE_SEMI_TRANSPARENT = 1, 
    OBJ_MODE_WINDOW = 2,
};

enum OBJ_SHAPE {
    OBJ_SHAPE_SQUARE = 0,
    OBJ_SHAPE_HORIZONTAL = 1,
    OBJ_SHAPE_VERTICAL = 2
};

enum OBJ_SIZE {
    OBJ_SIZE_8 = 0,
    OBJ_SIZE_16 = 1,
    OBJ_SIZE_32 = 2,
    OBJ_SIZE_64 = 3
};

struct oam_regular {
    /* 0-7 */ unsigned int y : 8;
    /*FALSE*/ bool affine_enabled : 1;
    /* 9   */ bool disabled : 1;
    /*10-11*/ enum OBJ_MODE mode : 2;
    /* 12  */ bool mosaic_enabled : 1;
    /* 13  */ bool palette_256 : 1;
    /*14-15*/ enum OBJ_SHAPE shape : 2;

    /* 0-8 */ unsigned int x : 9;
    /* 9-11*/ unsigned int __reserved : 3;
    /* 12  */ bool horizontal_flip : 1;
    /* 13  */ bool vertical_flip : 1;
    /*14-15*/ enum OBJ_SIZE size : 2;

    /* 0-9 */ unsigned int tile : 10;
    /*10-11*/ unsigned int priority : 2;
    /*12-15*/ unsigned int pallette : 3;

    unsigned int __affine_param_space : 16;
} PK_AL(4);

const struct oam_regular oam_norot_zero = { .affine_enabled = 0 };

struct oam_affine {
    /* 0-7 */ unsigned int y : 8;
    /* TRUE*/ bool affine_enabled : 1;
    /* 9   */ bool double_size_enabled : 1;
    /*10-11*/ enum OBJ_MODE mode : 2;
    /* 12  */ bool mosaic_enabled : 1;
    /* 13  */ bool palette_256 : 1;
    /*14-15*/ enum OBJ_SHAPE shape : 2;

    /* 0-8 */ unsigned int x : 9;
    /* 9-13*/ unsigned int affine_param_index : 5;
    /*14-15*/ enum OBJ_SIZE size : 2;

    /* 0-9 */ unsigned int tile : 10;
    /*10-11*/ unsigned int priority : 2;
    /*12-15*/ unsigned int pallette : 4;

    unsigned int __affine_param_space : 16;
} PK_AL(4);

const struct oam_affine oam_rot_zero = { .affine_enabled = 1 };

u32* OAM = (u32*)0x07000000;

//#

//# OAM Rotation/Scaling Params

struct oam_affine_param {
    u16 __filler1[3];
    fixed16 pa;
    u16 __filler2[3];
    fixed16 pb;
    u16 __filler3[3];
    fixed16 pc;
    u16 __filler4[3];
    fixed16 pd;
} PK_AL(4);

//#
