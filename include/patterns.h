#pragma once

#include "types.h"

struct pattern_data {
    unsigned int index : 4;
    bool a : 1;
    bool b : 1;
    bool c : 1;
    bool d : 1;
    bool e : 1;
    bool f : 1;
};

void pattern_draw_start();

void pattern_draw_finish();

void pattern_draw(struct pattern_data* pattern);