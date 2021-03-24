#pragma once

#include "types.h"
#include "macros.h"

typedef struct Color {
	unsigned int r : 5;
	unsigned int b : 5;
	unsigned int g : 5;
	bool _unused : 1;
} PK_AL(2) Color;

u8 Lerp8(u8 A, u8 B, u8 F);

Color mixcolor(Color a, Color b, u8 mix);