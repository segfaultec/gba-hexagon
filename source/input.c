#include "input.h"

// 0 - DOWN
// 1 - UP

static volatile struct KeyState* __key_state = (volatile struct KeyState*)0x4000130;

static struct KeyState __current_key_state_value __attribute((unused));

static struct KeyState __last_key_state_value __attribute__((unused));

struct KeyState* const __last_key_state __attribute__((unused)) = &__last_key_state_value;

struct KeyState* const __current_key_state __attribute((unused)) = &__current_key_state_value;

void UpdateKeyDown() {
	*__last_key_state = __current_key_state_value;
	__current_key_state_value = *__key_state;
}