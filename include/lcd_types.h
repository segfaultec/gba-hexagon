#pragma once

#include "macros.h"
#include "types.h"
#include "color.h"

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

static const struct dispcnt dispcnt_zero = {};

struct bgcnt {
    /* 0-1 */ unsigned int priority : 2;
    /* 2-3 */ unsigned int char_base_16k_block : 2;
    /* 4-5 */ char __reserved1 : 2;
    /* 6   */ bool mosaic : 1;
    /* 7   */ bool palette_256 : 1;
    /* 8-12*/ unsigned int screen_base_2k_block : 5;
    /* 13  */ bool affine_display_wrap : 1;
    /*14-15*/ unsigned int screen_size : 2;
} PK_AL(2);

static const struct bgcnt bgcnt_zero = {};

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

static const struct oam_regular oam_regular_zero = { 
    .affine_enabled = 0
};

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

static const struct oam_affine oam_affine_zero = {
    .affine_enabled = 1,
};

//#

//# OAM/BG Rotation/Scaling Params

struct bgscr {
    unsigned int x: 9;
    unsigned int __unused1 : 7;
    unsigned int y: 9;
    unsigned int __unused2 : 7;
} PK_AL(2);

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

struct bg_affine_param {
    fixed16 pa;
    fixed16 pb;
    fixed16 pc;
    fixed16 pd;
    fixed32 dx;
    fixed32 dy;
} PK_AL(4);

//#

//# LCD Const Pointers

static u32* const VRAM_BASE = (u32*)0x06000000;

static u32* const OAM = (u32*)0x07000000;

static struct dispcnt* const DISCNT = (struct dispcnt*)0x4000000;

static struct bgcnt* const BGCNT0 = (struct bgcnt*)0x4000008;
static struct bgcnt* const BGCNT1 = (struct bgcnt*)0x400000A;
static struct bgcnt* const BGCNT2 = (struct bgcnt*)0x400000C;
static struct bgcnt* const BGCNT3 = (struct bgcnt*)0x400000E;

INLINE struct bgmap* BG_MAP(u32 index) {
    return (struct bgmap*)(VRAM_BASE + (index << 11));
}

static volatile Color* const BG_PALETTE = (volatile Color*)0x05000000;

static u32* const BG_BMP_FRAME0 = (u32*)0x6000000;
static u32* const BG_BMP_FRAME1 = (u32*)0x600A000;

static struct bgscr* const BGSCR0 = (struct bgscr*)0x4000010;
static struct bgscr* const BGSCR1 = (struct bgscr*)0x4000014;
static struct bgscr* const BGSCR2 = (struct bgscr*)0x4000018;
static struct bgscr* const BGSCR3 = (struct bgscr*)0x400001C;

static volatile struct bg_affine_param* BGAFFINE2 = (volatile struct bg_affine_param*)0x4000020;

static volatile Color* const OAM_PALETTE = (volatile Color*)0x5000200;
//#