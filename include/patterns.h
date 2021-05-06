#pragma once

#include "types.h"

#define PATTERN_QUEUE_SIZE 8

void pq_init();
u8 pq_get(u8);
void pq_push(u8);

void pattern_draw_start(unsigned int subindex, u8 center_pattern_mask);

void pattern_draw_finish();

void pattern_draw(u8 index, u8 pattern_mask);