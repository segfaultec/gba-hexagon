#pragma once

#define CHECK_BIT(var,pos) ((var) & (1<<(pos)))

#define REINT_CAST(type, var) *(type*)&var

#define PK_AL(align) __attribute__((aligned(align), packed))
#define PK __attribute__((packed))

#define INLINE static inline

// #define ZERO_PTR(ptr) memset(ptr, 0, sizeof(*ptr))
// #define struct16(name, inner) struct name {inner} PK_AL(2)

// #define struct16z(name, inner) struct16(name, inner) const static name##_zero = {0}