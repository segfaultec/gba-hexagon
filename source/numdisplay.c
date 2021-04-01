#include "numdisplay.h"

#include "lcd_types.h"

#include <gba_systemcalls.h>

#include "numbers_img_bin.h"

static void* font_pos;
static volatile struct bgmap* map_pos;
static u32 palette_index;


void numdisplay_init(u32 tile_index, u32 map_index, u32 pal_index) {
    font_pos = (void*)VRAM_BASE + (tile_index << 14);
    map_pos = (void*)VRAM_BASE + (map_index << 11);
    palette_index = pal_index;

    CpuFastSet(numbers_img_bin, font_pos, COPY32 | (numbers_img_bin_size / 4));

    BGCNT0->char_base_16k_block = tile_index;
    BGCNT0->screen_base_2k_block = map_index;

    volatile Color* pal_start = BG_PALETTE + (pal_index * 16);
    pal_start[0] = black;
    pal_start[1] = white;
}

static s32 split_number(u32 number, u8* indexes) {
    u8 index = 7;
    do {
        indexes[index] = (number % 10) + 1;
        number /= 10;
        if (index == 0)
            break;
        else
            index--;
    } while (number > 0);
    return index + 1;
}

static void display_indexes(u8* indexes, s32 mainoffset, s32 tileoffset) {
    for (int i=0; i<8; i++) {
        struct bgmap tile = {
            .tile = 0,
            .palette = palette_index
        };
        map_pos[i + mainoffset] = tile;
    }
    for (int i=0; i<8; i++) {
        struct bgmap tile = {
            .tile = indexes[i],
            .palette = palette_index
        };
        map_pos[i + mainoffset + tileoffset] = tile;
    }
}

void numdisplay_update(u32 index, u32 number) {
    u8 indexes[8] = {0,0,0,0,0,0,0,0};
    
    s32 count = split_number(number, indexes);
    display_indexes(indexes, (32*index), -count);
}