#include <stdint.h>

#include "solution.h"

static uint8_t g_window[PAGE_SIZE] __attribute__((aligned(PAGE_SIZE)));
static uint8_t g_window_train[PAGE_SIZE] __attribute__((aligned(PAGE_SIZE)));
static uint8_t g_train_in1[PAGE_SIZE] __attribute__((aligned(PAGE_SIZE)));
static uint8_t g_train_in2[PAGE_SIZE] __attribute__((aligned(PAGE_SIZE)));
static uint8_t g_train_out[PAGE_SIZE] __attribute__((aligned(PAGE_SIZE)));

static inline void window_delay(void) {
    uint64_t x = 1;
    for (int i = 0; i < GATE_WINDOW_DELAY_ITERS; i++) {
        x = x * 33 + 17;
    }
    asm volatile("" : "+r"(x) : : "memory");
}

__attribute__((noinline))
void AND(void *out, void *in1, void *in2) {
    g_window_train[0] = 1;
    g_train_in1[0] = 1;
    g_train_in2[0] = 1;

    clflush_line(g_window);

    for (int iter = 0; iter < GATE_WINDOW_TRAIN_ITERS + 1; iter++) {
        volatile uint8_t *win =
            (iter < GATE_WINDOW_TRAIN_ITERS) ? g_window_train : g_window;
        volatile uint8_t *p1 =
            (iter < GATE_WINDOW_TRAIN_ITERS) ? g_train_in1 : (volatile uint8_t *)in1;
        volatile uint8_t *p2 =
            (iter < GATE_WINDOW_TRAIN_ITERS) ? g_train_in2 : (volatile uint8_t *)in2;
        volatile uint8_t *out_ptr =
            (iter < GATE_WINDOW_TRAIN_ITERS) ? g_train_out : (volatile uint8_t *)out;

        asm volatile("" ::: "memory");
        if (*win == 0) {
            continue;
        }

        window_delay();

        uint8_t v1 = *p1;
        uint8_t v2 = *p2;
        uint8_t mask = (uint8_t)(v1 & v2);
        volatile uint8_t *out_dep = out_ptr + (mask & 1);
        maccess((void *)out_dep);
    }
}

__attribute__((noinline))
void OR(void *out, void *in1, void *in2) {
    g_window_train[0] = 1;
    g_train_in1[0] = 1;
    g_train_in2[0] = 1;

    clflush_line(g_window);

    for (int iter = 0; iter < GATE_WINDOW_TRAIN_ITERS + 1; iter++) {
        volatile uint8_t *win =
            (iter < GATE_WINDOW_TRAIN_ITERS) ? g_window_train : g_window;
        volatile uint8_t *p1 =
            (iter < GATE_WINDOW_TRAIN_ITERS) ? g_train_in1 : (volatile uint8_t *)in1;
        volatile uint8_t *p2 =
            (iter < GATE_WINDOW_TRAIN_ITERS) ? g_train_in2 : (volatile uint8_t *)in2;
        volatile uint8_t *out_ptr =
            (iter < GATE_WINDOW_TRAIN_ITERS) ? g_train_out : (volatile uint8_t *)out;

        asm volatile("" ::: "memory");
        if (*win == 0) {
            continue;
        }

        window_delay();

        uint8_t v1 = *p1;
        volatile uint8_t *out1 = out_ptr + (v1 & 1);
        maccess((void *)out1);

        uint8_t v2 = *p2;
        volatile uint8_t *out2 = out_ptr + (v2 & 1);
        maccess((void *)out2);
    }
}
