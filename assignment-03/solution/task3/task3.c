#include <stdint.h>

#include "solution.h"

#define TRAIN_OUT_MAX 64

static uint8_t g_train_in[PAGE_SIZE] __attribute__((aligned(PAGE_SIZE)));
static uint8_t g_train_in1[PAGE_SIZE] __attribute__((aligned(PAGE_SIZE)));
static uint8_t g_train_in2[PAGE_SIZE] __attribute__((aligned(PAGE_SIZE)));
static uint8_t g_train_out_pool[TRAIN_OUT_MAX * PAGE_SIZE] __attribute__((aligned(PAGE_SIZE)));

static inline void *train_out_ptr(int idx) {
    int slot = idx % TRAIN_OUT_MAX;
    return (void *)(g_train_out_pool + (size_t)slot * PAGE_SIZE);
}

static inline void gate_delay(void) {
    uint64_t x = 1;
    for (int i = 0; i < GATE_DELAY_ITERS; i++) {
        x = x * 33 + 17;
    }
    asm volatile("" : "+r"(x) : : "memory");
}

__attribute__((noinline))
void NOT_M(void **outs, int n_out, void *in) {
    g_train_in[0] = 1;

    for (int iter = 0; iter < GATE_TRAINING_ITERS + 1; iter++) {
        volatile uint8_t *in_ptr =
            (iter < GATE_TRAINING_ITERS) ? g_train_in : (volatile uint8_t *)in;

        asm volatile("" ::: "memory");
        if (*in_ptr == 0) {
            continue;
        }
        gate_delay();
        for (int i = 0; i < n_out; i++) {
            void *out_ptr = (iter < GATE_TRAINING_ITERS) ? train_out_ptr(i) : outs[i];
            maccess(out_ptr);
        }
    }
}

__attribute__((noinline))
void NAND_M(void **outs, int n_out, void *in1, void *in2) {
    g_train_in1[0] = 1;
    g_train_in2[0] = 1;

    for (int iter = 0; iter < GATE_TRAINING_ITERS + 1; iter++) {
        volatile uint8_t *p1 =
            (iter < GATE_TRAINING_ITERS) ? g_train_in1 : (volatile uint8_t *)in1;
        volatile uint8_t *p2 =
            (iter < GATE_TRAINING_ITERS) ? g_train_in2 : (volatile uint8_t *)in2;

        asm volatile("" ::: "memory");
        if (((uint8_t)(*p1 + *p2)) == 0) {
            continue;
        }
        gate_delay();
        for (int i = 0; i < n_out; i++) {
            void *out_ptr = (iter < GATE_TRAINING_ITERS) ? train_out_ptr(i) : outs[i];
            maccess(out_ptr);
        }
    }
}

__attribute__((noinline))
void NOR_M(void **outs, int n_out, void *in1, void *in2) {
    g_train_in1[0] = 1;
    g_train_in2[0] = 1;

    for (int iter = 0; iter < GATE_TRAINING_ITERS + 1; iter++) {
        volatile uint8_t *p1 =
            (iter < GATE_TRAINING_ITERS) ? g_train_in1 : (volatile uint8_t *)in1;
        volatile uint8_t *p2 =
            (iter < GATE_TRAINING_ITERS) ? g_train_in2 : (volatile uint8_t *)in2;

        asm volatile("" ::: "memory");
        if (*p1 == 0) {
            continue;
        }
        if (*p2 == 0) {
            continue;
        }
        gate_delay();
        for (int i = 0; i < n_out; i++) {
            void *out_ptr = (iter < GATE_TRAINING_ITERS) ? train_out_ptr(i) : outs[i];
            maccess(out_ptr);
        }
    }
}
