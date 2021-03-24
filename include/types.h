#pragma once

#include "macros.h"

//# Int defs
#include <stdint.h>
#include <stdbool.h>

typedef	uint8_t		u8;	/**< Unsigned 8 bit value	*/
typedef	uint16_t	u16;	/**< Unsigned 16 bit value	*/
typedef	uint32_t	u32;	/**< Unsigned 32 bit value	*/
typedef	int8_t		s8;	/**< Signed 8 bit value	*/
typedef	int16_t		s16;	/**< Signed 16 bit value	*/
typedef	int32_t		s32;	/**< Signed 32 bit value	*/
typedef	volatile u8	vu8;	/**< volatile Unsigned 8 bit value	*/
typedef	volatile u16	vu16;	/**< volatile Unigned 16 bit value	*/
typedef	volatile u32	vu32;	/**< volatile Unsigned 32 bit value	*/
typedef	volatile s8	vs8;	/**< volatile Signed 8 bit value	*/
typedef	volatile s16	vs16;	/**< volatile Signed 8 bit value	*/
typedef	volatile s32	vs32;	/**< volatile Signed 8 bit value	*/
//#

//# Fixed point
// GBA 8.8 Fixed point decimal format

// Used https://www.coranac.com/tonc/text/fixed.htm#sec-fmath
// to help with conversion functions.

typedef s32 fixed32;
typedef s16 fixed16;

#define FIX_SHIFT 8
#define FIX_SCALE 256
#define FIX_MASK 255
#define FIX_SCALEF 256.0f

INLINE fixed32 fx_from_int(int d) {
    return d << FIX_SHIFT;
}

INLINE fixed32 fx_from_float(float f) {
    return (fixed32)(f*FIX_SCALE);
}

INLINE u32 fx_integer_part(fixed32 fx) {
    return fx >> FIX_SHIFT;
}

INLINE u32 fx_fractional_part(fixed32 fx) {
    return fx & FIX_MASK;
}

INLINE int fx_to_int(fixed32 fx) {
    return fx / FIX_SCALE;
}

INLINE float fx_to_float(fixed32 fx) {
    return fx / FIX_SCALEF;
}

INLINE fixed32 fx_multiply(fixed32 fa, fixed32 fb) {
    return (fa*fb)>>FIX_SHIFT;
}

INLINE fixed32 fx_division(fixed32 fa, fixed32 fb) {
    return ((fa)*FIX_SCALE)/(fb);
}
//#

//#

typedef struct Point {
    u16 x;
    u16 y;
} Point;

//#