#include <stdint.h>

#include "solution.h"

static uint8_t g_train_in[PAGE_SIZE] __attribute__((aligned(PAGE_SIZE)));
static uint8_t g_train_out[PAGE_SIZE] __attribute__((aligned(PAGE_SIZE)));

static inline void gate_delay(void) {
    uint64_t x = 1;
    for (int i = 0; i < GATE_DELAY_ITERS; i++) {
        x = x * 33 + 17; // Just some operation that depends on x
    }
    asm volatile("" : "+r"(x) : : "memory");
}

__attribute__((noinline))
void NOT(void *out, void *in) {
    g_train_in[0] = 0;

    for (int iter = 0; iter < GATE_TRAINING_ITERS + 1; iter++) {
        volatile uint8_t *in_ptr =
            (iter < GATE_TRAINING_ITERS) ? g_train_in : (volatile uint8_t *)in;
        volatile uint8_t *out_ptr =
            (iter < GATE_TRAINING_ITERS) ? g_train_out : (volatile uint8_t *)out;

        asm volatile("" ::: "memory");
        if (*in_ptr == 0) {
            // Architecturally taken, but predictor should guess not-taken
            continue;
        } else {
            gate_delay();
            maccess((void *)out_ptr);
        }
    }
}