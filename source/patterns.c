
#include "patterns.h"

#include "hexagon_map_bin.h"
#include "hexagon_reduced_img_bin.h"

#include "pedge_mid_br_img_bin.h"
#include "pedge_mid_bl_img_bin.h"
#include "pedge_mid_ul_img_bin.h"
#include "pedge_mid_ur_img_bin.h"
#include "pedge_vert_r_img_bin.h"
#include "pedge_vert_l_img_bin.h"

#include "pedge_diag_br_img_bin.h"
#include "pedge_diag_bl_img_bin.h"
#include "pedge_diag_ul_img_bin.h"
#include "pedge_diag_ur_img_bin.h"

#include "hexagon_center_gen_img_bin.h"

#include "lcd_impl.h"
#include "input.h"
#include "numdisplay.h"

#include <gba_systemcalls.h>

//#
static u8* base_map = (void*)(0x6000000 + (31<<11));

INLINE void* TileToPtr(u32 tile) {
    return VRAM_BASE + (tile << 4);
}

INLINE void* addr_from_tile(u8 x, u8 y) {
    return base_map + ((y * 32) + x);
}

INLINE u32 index_from_tile(u8 x, u8 y) {
    u32 toreturn = (((y << 5) | x));
    return toreturn;
}

INLINE void write_to_tile(u8* map, u8 x, u8 y, u8 value) {
    if (x > 32) { return; }
    if (y > 32) { return; }

    u32 index = index_from_tile(x, y);
    map[index] = value;
}
//#

// section 18 * 4 = 72
static const u32 pedge_diag_br_pos = 25; // size 6 * 3 = 18
static const u32 pedge_diag_bl_pos = 43; // size 6 * 3 = 18
static const u32 pedge_diag_ul_pos = 61; // size 6 * 3 = 18
static const u32 pedge_diag_ur_pos = 79; // size 6 * 3 = 18

// section 5 * 4 = 20
static const u32 pedge_mid_br_pos = 97;  // size 5
static const u32 pedge_mid_bl_pos = 102; // size 5
static const u32 pedge_mid_ul_pos = 107; // size 5
static const u32 pedge_mid_ur_pos = 112; // size 5

static const u32 pedge_vert_r_pos = 117; // size 2
static const u32 pedge_vert_l_pos = 119; // size 2

static const u32 hexagon_center_pos = 121; // size 12

static u8 current_subindex;
static u8 current_map[1024];

#define DRAW_MID_CHECK diag_offset != 0

static void draw_rm(register unsigned int index) {
    unsigned int current_x = 17 + (index*2);
    unsigned int current_y = 17 + index;

    int vert_start_x = current_x;
    int vert_start_y = current_y;
    if (current_subindex <= 5) {
        vert_start_x -= 1;
        vert_start_y -= 2;
    } else if (current_subindex <= 13) {
        vert_start_y -= 1;
    } else {
        vert_start_x += 1;
        vert_start_y -= 1;
    }

    if (!KEY_HELD(B)) {
        int line_amount = index*2 - 1;
        if (current_subindex >= 6) line_amount += 2;

        for (int i=0; i<line_amount; i++) {
            write_to_tile(current_map, vert_start_x, vert_start_y-i, pedge_vert_r_pos + 0);
            write_to_tile(current_map, vert_start_x+1, vert_start_y-i, pedge_vert_r_pos + 1);
        }
    }
}

static void draw_lm(register unsigned int index) {
    unsigned int current_x = 14 - (index*2);
    unsigned int current_y = 17 + index;

    int vert_start_x = current_x;
    int vert_start_y = current_y;
    if (current_subindex <= 5) {
        vert_start_x -= 0;
        vert_start_y -= 2;
    } else if (current_subindex <= 9) {
        vert_start_x -= 1;
        vert_start_y -= 2;
    } else if (current_subindex <= 13) {
        vert_start_x -= 1;
        vert_start_y -= 1;
    } else {
        vert_start_x -= 2;
        vert_start_y -= 1;
    }

    if (!KEY_HELD(B)) {
        int line_amount = index*2 - 1;
        if (current_subindex >= 10) line_amount += 2;

        for (int i=0; i<line_amount; i++) {
            write_to_tile(current_map, vert_start_x, vert_start_y-i, pedge_vert_l_pos + 0);
            write_to_tile(current_map, vert_start_x+1, vert_start_y-i, pedge_vert_l_pos + 1);
        }
    }
}

static void draw_br(register unsigned int index, unsigned int diag_offset) {
    unsigned int current_x = 17 + (index*2);
    unsigned int current_y = 17 + index;

    //# == BR BORDER ==
    unsigned int diag_x = current_x;
    unsigned int diag_y = current_y;

    // Correct offsets
    if (current_subindex <= 7) {
        
    } else if (current_subindex <= 9) {
        diag_x+=1;
    } else {
        diag_x+=1;
        diag_y+=1;
    }

    if (!KEY_HELD(A)) {
        if (current_subindex == 10 || current_subindex == 11) {
            write_to_tile(current_map, diag_x-2, diag_y-1, pedge_diag_br_pos + diag_offset + 0);
        }
        write_to_tile(current_map, diag_x-1, diag_y-1, pedge_diag_br_pos + diag_offset + 1);
        write_to_tile(current_map, diag_x, diag_y-1, pedge_diag_br_pos + diag_offset + 2);
        write_to_tile(current_map, diag_x-2, diag_y, pedge_diag_br_pos + diag_offset + 3);
        write_to_tile(current_map, diag_x-1, diag_y, pedge_diag_br_pos + diag_offset + 4);
        write_to_tile(current_map, diag_x, diag_y, pedge_diag_br_pos + diag_offset + 5);
    }

    //#
    
    //# == BR MID LINE == 

    // Don't draw the mid line if we are only drawing the vertical section
    if (DRAW_MID_CHECK) {
        int mid_start_x = current_x;
        int mid_start_y = current_y;
        int tile_index;

        // Adjust starting position and tile
        // to fit the diagonal section
        if (current_subindex <= 3) {
            mid_start_x -= 3;
            mid_start_y += 0;
            tile_index = 0;
        } else if (current_subindex <= 7) {
            mid_start_x -= 4;
            mid_start_y += 1;
            tile_index = 4;
        } else if (current_subindex <= 9) {
            mid_start_x -= 3;
            mid_start_y += 1;
            tile_index = 3;
        } else if (current_subindex <= 11) {
            mid_start_x -= 3;
            mid_start_y += 1;
            tile_index = 1;
        } else {
            mid_start_x -= 2;
            mid_start_y += 1;
            tile_index = 0;
        }

        if (!KEY_HELD(B)) {
            // Do the first row
            for (int dx = 0; dx <= tile_index; dx++) {
                if (mid_start_x + dx >= 16)
                    write_to_tile(current_map, mid_start_x + dx, mid_start_y, pedge_mid_br_pos + dx);
            }

            while (mid_start_x >= 14) {
                mid_start_x -= 2;
                mid_start_y += 1;

                for (int dx = 0; dx <= 4; dx++) {
                    if (mid_start_x + dx >= 16)
                        write_to_tile(current_map, mid_start_x  + dx, mid_start_y, pedge_mid_br_pos + dx);
                }   
            }
        }
    }
    
    //#
}

static void draw_bl(register unsigned int index, unsigned int diag_offset) {
    unsigned int current_x = 14 - (index*2);
    unsigned int current_y = 17 + index;

    //# == BL BORDER ==
    unsigned int diag_x = current_x;
    unsigned int diag_y = current_y;

    // Correct offsets
    if (current_subindex <= 7) {
        
    } else if (current_subindex <= 9) {
        diag_x-=1;
    } else {
        diag_x-=1;
        diag_y+=1;
    }

    if (!KEY_HELD(A)) {
        write_to_tile(current_map, diag_x, diag_y-1, pedge_diag_bl_pos + diag_offset + 0);
        write_to_tile(current_map, diag_x+1, diag_y-1, pedge_diag_bl_pos + diag_offset + 1);

        // Tile 2 only needs to be loaded 8-16, and if it is loaded earlier it clips into the center hexagon
        if (current_subindex == 10 || current_subindex == 11) {
            write_to_tile(current_map, diag_x+2, diag_y-1, pedge_diag_bl_pos + diag_offset + 2);
        }

        write_to_tile(current_map, diag_x, diag_y, pedge_diag_bl_pos + diag_offset + 3);
        write_to_tile(current_map, diag_x+1, diag_y, pedge_diag_bl_pos + diag_offset + 4);
        write_to_tile(current_map, diag_x+2, diag_y, pedge_diag_bl_pos + diag_offset + 5);
    }

    //#

    //# == BL MID LINE == 

    // Don't draw the mid line if we are only drawing the vertical section
    if (DRAW_MID_CHECK) {

        int mid_start_x = current_x;
        int mid_start_y = current_y;
        int tile_index;

        // Adjust starting position and tile
        // to fit the diagonal section
        if (current_subindex <= 3) {
            mid_start_x -= 1;
            mid_start_y += 0;
            tile_index = 4;
        } else if (current_subindex <= 7) {
            mid_start_x += 0;
            mid_start_y += 1;
            tile_index = 1;
        } else if (current_subindex <= 9) {
            mid_start_x -= 1;
            mid_start_y += 1;
            tile_index = 1;
        } else if (current_subindex <= 11) {
            mid_start_x -= 1;
            mid_start_y += 1;
            tile_index = 3;
        } else {
            mid_start_x -= 2;
            mid_start_y += 1;
            tile_index = 4;
        }

        if (!KEY_HELD(B)) {
            // Do the first row
            for (int dx = tile_index; dx <= 4; dx++) {
                if (mid_start_x + dx <= 15)
                    write_to_tile(current_map, mid_start_x + dx, mid_start_y, pedge_mid_bl_pos + dx);
            }

            while (mid_start_x <= 15) {
                mid_start_x += 2;
                mid_start_y += 1;

                for (int dx = 0; dx <= 4; dx++) {
                    if (mid_start_x + dx <= 15)
                        write_to_tile(current_map, mid_start_x + dx, mid_start_y, pedge_mid_bl_pos + dx);
                }   
            }
        }
    }
    
    //#
}

static void draw_ul(register unsigned int index, unsigned int diag_offset) {
    unsigned int current_x = 14 - (index*2);
    unsigned int current_y = 15 - index;

    //# == UL BORDER ==
    unsigned int diag_x = current_x;
    unsigned int diag_y = current_y;    

    if (current_subindex <= 7) {

    } else if (current_subindex <= 9) {
        diag_x -= 1;
    } else {
        diag_x -= 1;
        diag_y -= 1;
    }
    

    if (!KEY_HELD(A)) {
        write_to_tile(current_map, diag_x, diag_y, pedge_diag_ul_pos + diag_offset + 0);
        write_to_tile(current_map, diag_x+1, diag_y, pedge_diag_ul_pos + diag_offset + 1);
        write_to_tile(current_map, diag_x+2, diag_y, pedge_diag_ul_pos + diag_offset + 2);

        write_to_tile(current_map, diag_x, diag_y+1, pedge_diag_ul_pos + diag_offset + 3);

        write_to_tile(current_map, diag_x+1, diag_y+1, pedge_diag_ul_pos + diag_offset + 4);

        if (current_subindex == 10 || current_subindex == 11)
            write_to_tile(current_map, diag_x+2, diag_y+1, pedge_diag_ul_pos + diag_offset + 5);
    }

    //#

    //# == UL MID LINE ==

    // Don't draw the mid line if we are only drawing the vertical section
    if (DRAW_MID_CHECK) {

        int mid_start_x = current_x;
        int mid_start_y = current_y;
        int tile_index;

        if (current_subindex <= 3) {
            mid_start_x -= 1;
            mid_start_y -= 0;
            tile_index = 4;
        } else if (current_subindex <= 7) {
            mid_start_x -= 0;
            mid_start_y -= 1;
            tile_index = 0;
        } else if (current_subindex <= 9) {
            mid_start_x -= 1;
            mid_start_y -= 1;
            tile_index = 1;
        } else if (current_subindex <= 11) {
            mid_start_x -= 1;
            mid_start_y -= 1;
            tile_index = 3;
        } else {
            mid_start_x -= 2;
            mid_start_y -= 1;
            tile_index = 4;
        }

        if (!KEY_HELD(B)) {
            // Do the first row
            for (int dx = tile_index; dx <= 4; dx++) {
                if (mid_start_x + dx <= 15)
                    write_to_tile(current_map, mid_start_x + dx, mid_start_y, pedge_mid_ul_pos + dx);
            }

            while (mid_start_x <= 15) {
                mid_start_x += 2;
                mid_start_y -= 1;

                for (int dx = 0; dx <= 4; dx++) {
                    if (mid_start_x + dx <= 15)
                        write_to_tile(current_map, mid_start_x + dx, mid_start_y, pedge_mid_ul_pos + dx);
                }

            }
        }
    }
    //#
}

static void draw_ur(register unsigned int index, unsigned int diag_offset) {
    unsigned int current_x = 17 + (2 * index);
    unsigned int current_y = 15 - index;

    //# == UR BORDER ==
    unsigned int diag_x = current_x;
    unsigned int diag_y = current_y;

    if (current_subindex <= 7) {

    } else if (current_subindex <= 9) {
        diag_x += 1;
    } else {
        diag_x += 1;
        diag_y -= 1;
    }

    if (!KEY_HELD(A)) {
        write_to_tile(current_map, diag_x-2, diag_y, pedge_diag_ur_pos + diag_offset + 0);
        write_to_tile(current_map, diag_x-1, diag_y, pedge_diag_ur_pos + diag_offset + 1);
        write_to_tile(current_map, diag_x, diag_y, pedge_diag_ur_pos + diag_offset + 2);

        if (current_subindex == 10 || current_subindex == 11)
            write_to_tile(current_map, diag_x-2, diag_y+1, pedge_diag_ur_pos + diag_offset + 3);

        write_to_tile(current_map, diag_x-1, diag_y+1, pedge_diag_ur_pos + diag_offset + 4);
        write_to_tile(current_map, diag_x, diag_y+1, pedge_diag_ur_pos + diag_offset + 5);
    }

    //#

    //# == UR MID LINE ==

    // Don't draw the mid line if we are only drawing the vertical section
    if (DRAW_MID_CHECK) {
        int mid_start_x = current_x;
        int mid_start_y = current_y;
        int tile_index;

        // Adjust starting position and tile
        // to fit the diagonal section
        if (current_subindex <= 3) {
            mid_start_x -= 3;
            mid_start_y -= 0;
            tile_index = 0;
        } else if (current_subindex <= 7) {
            mid_start_x -= 4;
            mid_start_y -= 1;
            tile_index = 4;
        } else if (current_subindex <= 9) {
            mid_start_x -= 3;
            mid_start_y -= 1;
            tile_index = 3;
        } else if (current_subindex <= 11) {
            mid_start_x -= 3;
            mid_start_y -= 1;
            tile_index = 1;
        } else {
            mid_start_x -= 2;
            mid_start_y -= 1;
            tile_index = 0;
        }

        if (!KEY_HELD(B)) {
            // Do the first row
            for (int dx = 0; dx <= tile_index; dx++) {
                if (mid_start_x + dx >= 16)
                    write_to_tile(current_map, mid_start_x + dx, mid_start_y, pedge_mid_ur_pos + dx);
            }

            while (mid_start_x >= 14) {
                mid_start_x -= 2;
                mid_start_y -= 1;

                for (int dx = 0; dx <= 4; dx++) {
                    if (mid_start_x + dx >= 16)
                        write_to_tile(current_map, mid_start_x + dx, mid_start_y, pedge_mid_ur_pos + dx);
                }   
            }
        }

    }

    //#
}

static void draw_center(register unsigned int index) {
    if (index >= 1) return;
    if (current_subindex <= 1) return;

    write_to_tile(current_map, 14, 15, hexagon_center_pos);
    write_to_tile(current_map, 15, 15, hexagon_center_pos+1);
    write_to_tile(current_map, 16, 15, hexagon_center_pos+2);
    write_to_tile(current_map, 17, 15, hexagon_center_pos+3);

    write_to_tile(current_map, 14, 16, hexagon_center_pos+4);
    write_to_tile(current_map, 15, 16, hexagon_center_pos+5);
    write_to_tile(current_map, 16, 16, hexagon_center_pos+6);
    write_to_tile(current_map, 17, 16, hexagon_center_pos+7);

    write_to_tile(current_map, 14, 17, hexagon_center_pos+8);
    write_to_tile(current_map, 15, 17, hexagon_center_pos+9);
    write_to_tile(current_map, 16, 17, hexagon_center_pos+10);
    write_to_tile(current_map, 17, 17, hexagon_center_pos+11);

}

void pattern_draw_start(unsigned int subindex, u8 center_pattern_mask) {
    // Load the base tiles
    CpuFastSet(hexagon_reduced_img_bin, VRAM_BASE, COPY32 | 400);

    // Initialise the WRAM map
    // I can't mess about with VRAM directly too well, poking with unaligned data there straight up doesn't work.
    // Instead I'm making a buffer in WRAM to mess about with, copying it whole into VRAM once I'm done.
    CpuFastSet(hexagon_map_bin, current_map, COPY32 | 256);
    current_subindex = subindex;

    int diag_subindex = subindex; // Diagonal is 16 frames
    int mid_subindex = subindex & 3; // Mid is 4 frames
    int vert_subindex = subindex & 7; // Vert is 8 frames

    int center_subindex = 15 - subindex;

    //# Load current tiles

    CpuFastSet( // Center tiles
        hexagon_center_gen_img_bin + (center_pattern_mask * 64 * 12 * 14) + (center_subindex * 64 * 12),
        TileToPtr(hexagon_center_pos),
        COPY32 | 16 * 12
    );

    CpuFastSet( // Mid BR
        pedge_mid_br_img_bin + (mid_subindex * 64 * 5),
        TileToPtr(pedge_mid_br_pos),
        COPY32 | 16 * 5
    );

    CpuFastSet( // Mid BL
        pedge_mid_bl_img_bin + (mid_subindex * 64 * 5),
        TileToPtr(pedge_mid_bl_pos),
        COPY32 | 16 * 5
    );

    CpuFastSet( // Mid UL
        pedge_mid_ul_img_bin + (mid_subindex * 64 * 5),
        TileToPtr(pedge_mid_ul_pos),
        COPY32 | 16 * 5
    );

    CpuFastSet( // Mid UR
        pedge_mid_ur_img_bin + (mid_subindex * 64 * 5),
        TileToPtr(pedge_mid_ur_pos),
        COPY32 | 16 * 5
    );

    CpuFastSet( // Diag BR
        pedge_diag_br_img_bin + (diag_subindex * 64 * 18),
        TileToPtr(pedge_diag_br_pos),
        COPY32 | 16 * 18
    );

    CpuFastSet( // Diag BL
        pedge_diag_bl_img_bin + (diag_subindex * 64 * 18),
        TileToPtr(pedge_diag_bl_pos),
        COPY32 | 16 * 18
    );

    CpuFastSet( // Diag UR
        pedge_diag_ur_img_bin + (diag_subindex * 64 * 18),
        TileToPtr(pedge_diag_ur_pos),
        COPY32 | 16 * 18
    );

    CpuFastSet( // Vert R
        pedge_vert_r_img_bin + (vert_subindex * 64 * 2),
        TileToPtr(pedge_vert_r_pos),
        COPY32 | 16 * 2
    );

    CpuFastSet( // Vert L
        pedge_vert_l_img_bin + (vert_subindex * 64 * 2),
        TileToPtr(pedge_vert_l_pos),
        COPY32 | 16 * 2
    );

    CpuFastSet( // Diag UL
        pedge_diag_ul_img_bin + (diag_subindex * 64 * 18),
        TileToPtr(pedge_diag_ul_pos),
        COPY32 | 16 * 18
    );
    //#
}

void pattern_draw_finish() {
    // Copy the WRAM map into VRAM
    CpuFastSet(current_map, base_map, COPY32 | 256);
}

void pattern_draw(u8 index, u8 mask) {

    if (mask == 0) return;

    if (index == 0 && current_subindex <= 1) return;

    if (CHECK_MASK(mask, 1 | 2)) {
        draw_ur(index, 12);
    } else if (CHECK_MASK(mask, 1)) {
        draw_ur(index, 6);
    } else if (CHECK_MASK(mask, 2)) {
        draw_ur(index, 0);
    }

    if (CHECK_MASK(mask, 2 | 4)) {
        draw_br(index, 12);
    } else if (CHECK_MASK(mask, 2)) {
        draw_br(index, 0);
    } else if (CHECK_MASK(mask, 4)) {
        draw_br(index, 6);
    }

    if (CHECK_MASK(mask, 8 | 16)) {
        draw_bl(index, 12);
    } else if (CHECK_MASK(mask, 8)) {
        draw_bl(index, 6);
    } else if (CHECK_MASK(mask, 16)) {
        draw_bl(index, 0);
    }

    if (CHECK_MASK(mask, 16 | 32)) {
        draw_ul(index, 12);
    } else if (CHECK_MASK(mask, 16)) {
        draw_ul(index, 0);
    } else if (CHECK_MASK(mask, 32)) {
        draw_ul(index, 6);
    }

    if (CHECK_MASK(mask, 2))
        draw_rm(index);
    if (CHECK_MASK(mask, 16))
        draw_lm(index);

    draw_center(index);
}