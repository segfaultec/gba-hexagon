#pragma once

#include "macros.h"
#include "types.h"

struct KeyState {
	bool A : 1;
	bool B : 1;
	bool Select : 1;
	bool Start : 1;
	bool Right : 1;
	bool Left : 1;
	bool Up : 1;
	bool Down : 1;
	bool R : 1;
	bool L : 1;
	unsigned int __unused : 6;
} PK_AL(2);

// 0 - DOWN
// 1 - UP

extern struct KeyState* const __current_key_state __attribute((unused));

extern struct KeyState* const __last_key_state __attribute__((unused));

void UpdateKeyDown();

#define KEY_DOWN(key) (__current_key_state->key == 0 && __last_key_state->key == 1)

#define KEY_HELD(key) (__current_key_state->key == 0)

#define KEY_UP(key) (__current_key_state->key == 1 && __last_key_state->key == 0)