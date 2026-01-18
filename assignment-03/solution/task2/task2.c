#include <stdint.h>

#include "solution.h"

static uint8_t g_train_in1[PAGE_SIZE] __attribute__((aligned(PAGE_SIZE)));
static uint8_t g_train_in2[PAGE_SIZE] __attribute__((aligned(PAGE_SIZE)));
static uint8_t g_train_out[PAGE_SIZE] __attribute__((aligned(PAGE_SIZE)));

static inline void gate_delay(void) {
    uint64_t x = 1;
    for (int i = 0; i < GATE_DELAY_ITERS; i++) {
        x = x * 33 + 17; // Just some operation that depends on x
    }
    asm volatile("" : "+r"(x) : : "memory");
}

__attribute__((noinline))
void NAND(void *out, void *in1, void *in2) {
    g_train_in1[0] = 0;
    g_train_in2[0] = 0;

    for (int iter = 0; iter < GATE_TRAINING_ITERS + 1; iter++) {
        volatile uint8_t *p1 =
            (iter < GATE_TRAINING_ITERS) ? g_train_in1 : (volatile uint8_t *)in1;
        volatile uint8_t *p2 =
            (iter < GATE_TRAINING_ITERS) ? g_train_in2 : (volatile uint8_t *)in2;
        volatile uint8_t *out_ptr =
            (iter < GATE_TRAINING_ITERS) ? g_train_out : (volatile uint8_t *)out;

        asm volatile("" ::: "memory");
        if (((uint8_t)(*p1 + *p2)) == 0) {
            // Architecturally taken, but predictor should guess not-taken
            continue;
        } else {
            gate_delay();
            maccess((void *)out_ptr);
        }
    }
}

__attribute__((noinline))
void NOR(void *out, void *in1, void *in2) {
    g_train_in1[0] = 0;
    g_train_in2[0] = 0;

    for (int iter = 0; iter < GATE_TRAINING_ITERS + 1; iter++) {
        volatile uint8_t *p1 =
            (iter < GATE_TRAINING_ITERS) ? g_train_in1 : (volatile uint8_t *)in1;
        volatile uint8_t *p2 =
            (iter < GATE_TRAINING_ITERS) ? g_train_in2 : (volatile uint8_t *)in2;
        volatile uint8_t *out_ptr =
            (iter < GATE_TRAINING_ITERS) ? g_train_out : (volatile uint8_t *)out;

        asm volatile("" ::: "memory");
        if (*p1 == 0 || *p2 == 0) {
            // Architecturally taken, but predictor should guess not-taken
            continue;
        } else {
            gate_delay();
            maccess((void *)out_ptr);
        }
    }
}
