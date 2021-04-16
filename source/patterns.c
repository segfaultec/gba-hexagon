
#include "patterns.h"

#include "hexagon_map_bin.h"
#include "hexagon_reduced_img_bin.h"
#include "pedge_mid_img_bin.h"
#include "pedge_diag_br_img_bin.h"

#include "lcd_impl.h"
#include "input.h"
#include "numdisplay.h"

#include <gba_systemcalls.h>

static u8* map = (void*)(0x6000000 + (31<<11));

static const u32 pedge_mid_pos = 25;
static const u32 pedge_diag_br_pos = 31;

INLINE void* TileToPtr(u32 tile) {
    return VRAM_BASE + (tile << 4);
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


const int index_min = 0;
const int index_max = 10;
const int subindex_min = 0;
const int subindex_max = 15;

static int index = 2;
static int subindex = 0;

void patterns_update() {

    if (KEY_DOWN(Up)) subindex--;
    if (KEY_DOWN(Down)) subindex++;

    if (subindex < subindex_min) {
        subindex = subindex_max;
        index--;
    } else if (subindex > subindex_max) {
        subindex = subindex_min;
        index++;
    }
    if (index < index_min) index = index_max;
    else if (index > index_max) index = index_min;

    numdisplay_update(0, index);
    numdisplay_update(1, subindex);

    int diag_subindex = subindex; // Diagonal is 16 frames
    int mid_subindex = subindex % 4; // Mid is 4 frames

    // Load current tiles
    CpuFastSet(
        pedge_mid_img_bin + (mid_subindex * 64 * 5),
        TileToPtr(pedge_mid_pos),
        COPY32 | 16 * 5
    );

    CpuFastSet(
        pedge_diag_br_img_bin + (diag_subindex * 64 * 6),
        TileToPtr(pedge_diag_br_pos),
        COPY32 | 16 * 6
    );

    // Initialise the WRAM map
    // I can't mess about with VRAM directly too well, poking with unaligned data there straight up doesn't work.
    // Instead I'm making a buffer in WRAM to mess about with, copying it whole into VRAM once I'm done.
    u8 current_map[1024];
    CpuFastSet(hexagon_map_bin, current_map, COPY32 | 256);

    struct diagcalc current = {
        17, 17, 1
    };

    // Find the starting position
    for (unsigned int i=0; i<index*3; i++) {
        CalcDiag(&current, 1, 1);
    }

    // == BR BORDER ==
    unsigned int diag_x = current.x;
    unsigned int diag_y = current.y;

    // Correct offsets
    if (subindex <= 7) {
        
    } else if (subindex <= 9) {
        diag_x+=1;
    } else {
        diag_x+=1;
        diag_y+=1;
    }

    if (!KEY_HELD(A)) {
        // Only needs to be loaded 8-16, and if it is loaded earlier it clips into the center hexagon
        if (subindex >= 8) {
            current_map[index_from_tile(diag_x-2, diag_y-1)] = pedge_diag_br_pos + 0;
        }
        current_map[index_from_tile(diag_x-1, diag_y-1)] = pedge_diag_br_pos + 1;
        current_map[index_from_tile(diag_x, diag_y-1)] = pedge_diag_br_pos + 2;
        current_map[index_from_tile(diag_x-2, diag_y)] = pedge_diag_br_pos + 3;
        current_map[index_from_tile(diag_x-1, diag_y)] = pedge_diag_br_pos + 4;
        current_map[index_from_tile(diag_x, diag_y)] = pedge_diag_br_pos + 5;
    }

    // == BR MID LINE == 


    // == BR VERT CAP ==

    while (current.x > 16) {
        CalcDiag(&current, -1, 1);
    }

    unsigned int cap_x = current.x;
    unsigned int cap_y = current.y;

    if (subindex <= 3) {
        current_map[index_from_tile(cap_x, cap_y-1)] = pedge_mid_pos + 0;
        current_map[index_from_tile(cap_x, cap_y)] = pedge_mid_pos + 2;
    } else if (subindex <= 7) {
        current_map[index_from_tile(cap_x, cap_y)] = pedge_mid_pos + 1;
        current_map[index_from_tile(cap_x, cap_y+1)] = pedge_mid_pos + 3;
    } else if (subindex <= 11) {
        current_map[index_from_tile(cap_x, cap_y)] = pedge_mid_pos + 0;
        current_map[index_from_tile(cap_x, cap_y+1)] = pedge_mid_pos + 2;
    } else {
        current_map[index_from_tile(cap_x, cap_y+1)] = pedge_mid_pos + 1;
        current_map[index_from_tile(cap_x, cap_y+2)] = pedge_mid_pos + 3;
    }

    // A missing dot similar to above
    if (subindex == 3) {
        current_map[index_from_tile(cap_x, cap_y+1)] = pedge_mid_pos + 4;
    }
    if (subindex == 11) {
        current_map[index_from_tile(cap_x, cap_y+2)] = pedge_mid_pos + 4;
    }

    // Copy the WRAM map into VRAM
    CpuFastSet(current_map, map, COPY32 | 256);
}