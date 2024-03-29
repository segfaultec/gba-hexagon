#include "color.h"

u8 Lerp8(u8 A, u8 B, u8 F) {
    if (F == 0) return A;
    if (F == 0xFF) return B;
    F++;
    return A + (u8)(((u16)(B-A) * F) >> 8);
}

Color mixcolor(Color a, Color b, u8 mix) {
	a.r = Lerp8(a.r, b.r, mix);
	a.g = Lerp8(a.g, b.g, mix);
	a.b = Lerp8(a.b, b.b, mix);
	return a;
}