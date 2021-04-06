
#include "patterns.h"

#include "hexagon_map_bin.h"
#include "hexagon_reduced_img_bin.h"
#include "pedge_test_img_bin.h"

#include "lcd_impl.h"
#include "input.h"
#include "numdisplay.h"

#include <gba_systemcalls.h>

static u8* map = (void*)(0x6000000 + (31<<11));

static const u32 pedge_position = 25;

INLINE void* TileToPtr(u32 tile) {
    return VRAM_BASE + (pedge_position << 4);
}

void patterns_init() {
    CpuFastSet(hexagon_reduced_img_bin, VRAM_BASE, COPY32 | (hexagon_reduced_img_bin_size >> 2));
}

INLINE void* addr_from_tile(u8 x, u8 y) {
    return map + ((y * 32) + x);
}

INLINE u32 index_from_tile(u8 x, u8 y) {
    return (y*32) + x;
}

struct diagcalc {
    unsigned int x;
    unsigned int y;
    unsigned int counter;
};

 // 0 -> change in x
 // 1 -> change in x
 // 2 -> change in y

// Set counter to 2 for there to be a Y change right away
INLINE void CalcDiag(struct diagcalc* data, const int dirX, const int dirY) {
    unsigned int count = ++data->counter;
    if (count == 1 || count == 2) {
        data->x += dirX;
    } else {
        data->counter = 0;
        data->y += dirY;
    }
}

static unsigned int x = 16;
static unsigned int y = 18;
static int subindex = 0;

void patterns_update() {

    if (KEY_DOWN(Up)) subindex--;
    if (KEY_DOWN(Down)) subindex++;

    if (subindex < 0) {
        subindex = 7;
        y--;
    }
    if (subindex > 7) {
        subindex = 0;
        y++;
    }
    if (y < 18) y = 25;
    if (y > 25) y = 18;

    numdisplay_update(0, y);
    numdisplay_update(1, subindex);

    // Load current tiles

    CpuFastSet(
        pedge_test_img_bin + (subindex * 384),
        TileToPtr(pedge_position),
        COPY32 | 96 /*6 8bpp tiles*/
    );

    //if (++index > 7) index = 0;

    // Initialise the WRAM map
    u8 current_map[1024];
    CpuFastSet(hexagon_map_bin, current_map, COPY32 | 256);

    // Edit it
    current_map[index_from_tile(x, y)] = pedge_position + 0;
    current_map[index_from_tile(x+1, y)] = pedge_position + 1;
    current_map[index_from_tile(x, y+1)] = pedge_position + 3;
    current_map[index_from_tile(x+1, y+1)] = pedge_position + 4;
    if (subindex == 7) {
        current_map[index_from_tile(x, y+2)] = pedge_position + 2;
        current_map[index_from_tile(x+1, y+2)] = pedge_position + 5;
    }
    else if (subindex > 2) {
        current_map[index_from_tile(x, y+2)] = pedge_position + 5;
    }

    // Copy the WRAM map into VRAM
    CpuFastSet(current_map, map, COPY32 | 256);
}