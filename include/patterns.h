#pragma once

#include "types.h"

struct pattern_data {
    unsigned int index : 4;
    unsigned int subindex : 4;
    bool a : 1;
    bool b : 1;
    bool c : 1;
    bool d : 1;
    bool e : 1;
    bool f : 1;
};

void patterns_init();

void draw_pattern(struct pattern_data* pattern);