#include "patterns.h"

static u8 pattern_queue[PATTERN_QUEUE_SIZE];

static unsigned int queue_front = 0;

INLINE u8 inc_queue_ptr(u8 queue_ptr, u8 amount) {
    queue_ptr += amount;
    while (queue_ptr >= PATTERN_QUEUE_SIZE)
        queue_ptr -= PATTERN_QUEUE_SIZE;
    return queue_ptr;
}

void pq_init() {
    for (int i=0; i<PATTERN_QUEUE_SIZE; i++)
        pattern_queue[i] = 0;
    queue_front = 0;
}

u8 pq_get(u8 index) {
    u8 true_index = inc_queue_ptr(queue_front, index);
    return pattern_queue[true_index];
}

void pq_push(u8 value) {
    pattern_queue[queue_front] = value;
    queue_front = inc_queue_ptr(queue_front, 1);
}