#include <stdint.h>

#include "solution.h"

#define XOR_SCRATCH_PAGES 32

static uint8_t g_xor_scratch[XOR_SCRATCH_PAGES * PAGE_SIZE] __attribute__((aligned(PAGE_SIZE)));

static inline void *xor_line(int idx) {
    return (void *)(g_xor_scratch + (size_t)idx * PAGE_SIZE);
}

static void xor_flush_scratch(int count) {
    for (int i = 0; i < count; i++) {
        clflush_line(xor_line(i));
    }
    asm volatile("mfence" ::: "memory");
}

__attribute__((noinline))
void XOR(void *out, void *in1, void *in2) {
    /* scratch layout */
    void *na1 = xor_line(0);
    void *na2 = xor_line(1);
    void *na_for_a = xor_line(2);
    void *a1 = xor_line(3);
    void *a2 = xor_line(4);

    void *nb1 = xor_line(5);
    void *nb2 = xor_line(6);
    void *nb_for_b = xor_line(7);
    void *b1 = xor_line(8);
    void *b2 = xor_line(9);

    void *t = xor_line(10);
    void *nt1 = xor_line(11);
    void *nt2 = xor_line(12);
    void *nt_for_t = xor_line(13);
    void *t1 = xor_line(14);
    void *t2 = xor_line(15);

    void *x = xor_line(16);
    void *y = xor_line(17);

    xor_flush_scratch(18);

    void *na_outs[3] = {na1, na2, na_for_a};
    NOT_M(na_outs, 3, in1);

    void *a_outs[2] = {a1, a2};
    NOT_M(a_outs, 2, na_for_a);

    void *nb_outs[3] = {nb1, nb2, nb_for_b};
    NOT_M(nb_outs, 3, in2);

    void *b_outs[2] = {b1, b2};
    NOT_M(b_outs, 2, nb_for_b);

    NAND(t, a1, b1);

    void *nt_outs[3] = {nt1, nt2, nt_for_t};
    NOT_M(nt_outs, 3, t);

    void *t_outs[2] = {t1, t2};
    NOT_M(t_outs, 2, nt_for_t);

    NAND(x, a2, t1);
    NAND(y, b2, t2);
    NAND(out, x, y);
}
