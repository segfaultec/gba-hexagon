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
} PK_AL(2);

// 0 - DOWN
// 1 - UP

const static struct KeyState* __key_state = (struct KeyState*)0x4000130;

static struct KeyState __last_key_state_value __attribute__((unused));

static struct KeyState* __last_key_state __attribute__((unused)) = &__last_key_state_value;

INLINE void UpdateKeyDown() {
	*__last_key_state = *__key_state;
}

#define KEY_DOWN(key) (__key_state->key == 0 && __last_key_state->key == 1)

#define KEY_HELD(key) (__key_state->key == 0)

#define KEY_UP(key) (__key_state->key == 1 && __last_key_state->key == 0)