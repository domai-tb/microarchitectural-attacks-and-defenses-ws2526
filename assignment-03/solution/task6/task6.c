#include <stdint.h>
#include <string.h>

#include "solution.h"

#define FANOUT_PER_LIT 16
#define SEG_SCRATCH_PAGES 256

typedef struct {
    uint8_t *pool;
    int      idx;
} scratch_pool_t;

static uint8_t g_seg_scratch[SEG_SCRATCH_PAGES * PAGE_SIZE] __attribute__((aligned(PAGE_SIZE)));

static inline void *scratch_alloc(scratch_pool_t *sp) {
    void *p = (void *)(sp->pool + (size_t)sp->idx * PAGE_SIZE);
    sp->idx++;
    return p;
}

static void scratch_flush(int count) {
    for (int i = 0; i < count; i++) {
        clflush_line(g_seg_scratch + (size_t)i * PAGE_SIZE);
    }
    asm volatile("mfence" ::: "memory");
}

static inline void *next_copy(void **arr, int *idx, int max) {
    void *p = arr[*idx % max];
    (*idx)++;
    return p;
}

static void or_reduce(void *out, void **terms, int n, scratch_pool_t *sp) {
    if (n == 1) {
        OR(out, terms[0], terms[0]);
        return;
    }
    if (n == 2) {
        OR(out, terms[0], terms[1]);
        return;
    }

    void *acc = scratch_alloc(sp);
    OR(acc, terms[0], terms[1]);
    for (int i = 2; i < n; i++) {
        void *next = (i == n - 1) ? out : scratch_alloc(sp);
        OR(next, acc, terms[i]);
        acc = next;
    }
}

static void and3(void *out, void *a, void *b, void *c, scratch_pool_t *sp) {
    void *tmp = scratch_alloc(sp);
    AND(tmp, a, b);
    AND(out, tmp, c);
}

__attribute__((noinline))
void seven_segment(void **out, void **in) {
    scratch_pool_t sp = {.pool = g_seg_scratch, .idx = 0};

    void *nx0[FANOUT_PER_LIT + 1];
    void *x0[FANOUT_PER_LIT];
    void *nx1[FANOUT_PER_LIT + 1];
    void *x1[FANOUT_PER_LIT];
    void *nx2[FANOUT_PER_LIT + 1];
    void *x2[FANOUT_PER_LIT];
    void *nx3[FANOUT_PER_LIT + 1];
    void *x3[FANOUT_PER_LIT];

    for (int i = 0; i < FANOUT_PER_LIT + 1; i++) {
        nx0[i] = scratch_alloc(&sp);
        nx1[i] = scratch_alloc(&sp);
        nx2[i] = scratch_alloc(&sp);
        nx3[i] = scratch_alloc(&sp);
    }
    for (int i = 0; i < FANOUT_PER_LIT; i++) {
        x0[i] = scratch_alloc(&sp);
        x1[i] = scratch_alloc(&sp);
        x2[i] = scratch_alloc(&sp);
        x3[i] = scratch_alloc(&sp);
    }

    int scratch_used = sp.idx;
    scratch_flush(scratch_used);

    NOT_M(nx0, FANOUT_PER_LIT + 1, in[0]);
    NOT_M(nx1, FANOUT_PER_LIT + 1, in[1]);
    NOT_M(nx2, FANOUT_PER_LIT + 1, in[2]);
    NOT_M(nx3, FANOUT_PER_LIT + 1, in[3]);

    NOT_M(x0, FANOUT_PER_LIT, nx0[FANOUT_PER_LIT]);
    NOT_M(x1, FANOUT_PER_LIT, nx1[FANOUT_PER_LIT]);
    NOT_M(x2, FANOUT_PER_LIT, nx2[FANOUT_PER_LIT]);
    NOT_M(x3, FANOUT_PER_LIT, nx3[FANOUT_PER_LIT]);

    int ix0 = 0, inx0 = 0;
    int ix1 = 0, inx1 = 0;
    int ix2 = 0, inx2 = 0;
    int ix3 = 0, inx3 = 0;

    /* Segment 0 */
    void *s0_terms[6];
    s0_terms[0] = scratch_alloc(&sp);
    AND(s0_terms[0], next_copy(nx0, &inx0, FANOUT_PER_LIT),
        next_copy(nx2, &inx2, FANOUT_PER_LIT));

    s0_terms[1] = scratch_alloc(&sp);
    AND(s0_terms[1], next_copy(nx0, &inx0, FANOUT_PER_LIT),
        next_copy(x3, &ix3, FANOUT_PER_LIT));

    s0_terms[2] = scratch_alloc(&sp);
    and3(s0_terms[2],
         next_copy(x0, &ix0, FANOUT_PER_LIT),
         next_copy(x2, &ix2, FANOUT_PER_LIT),
         next_copy(nx3, &inx3, FANOUT_PER_LIT),
         &sp);

    s0_terms[3] = scratch_alloc(&sp);
    and3(s0_terms[3],
         next_copy(nx1, &inx1, FANOUT_PER_LIT),
         next_copy(nx2, &inx2, FANOUT_PER_LIT),
         next_copy(x3, &ix3, FANOUT_PER_LIT),
         &sp);

    s0_terms[4] = scratch_alloc(&sp);
    AND(s0_terms[4], next_copy(x1, &ix1, FANOUT_PER_LIT),
        next_copy(x2, &ix2, FANOUT_PER_LIT));

    s0_terms[5] = scratch_alloc(&sp);
    AND(s0_terms[5], next_copy(x1, &ix1, FANOUT_PER_LIT),
        next_copy(nx3, &inx3, FANOUT_PER_LIT));

    or_reduce(out[0], s0_terms, 6, &sp);

    /* Segment 1 */
    void *s1_terms[5];
    s1_terms[0] = scratch_alloc(&sp);
    AND(s1_terms[0], next_copy(nx0, &inx0, FANOUT_PER_LIT),
        next_copy(nx1, &inx1, FANOUT_PER_LIT));

    s1_terms[1] = scratch_alloc(&sp);
    AND(s1_terms[1], next_copy(nx0, &inx0, FANOUT_PER_LIT),
        next_copy(x2, &ix2, FANOUT_PER_LIT));

    s1_terms[2] = scratch_alloc(&sp);
    and3(s1_terms[2],
         next_copy(nx1, &inx1, FANOUT_PER_LIT),
         next_copy(x2, &ix2, FANOUT_PER_LIT),
         next_copy(nx3, &inx3, FANOUT_PER_LIT),
         &sp);

    s1_terms[3] = scratch_alloc(&sp);
    AND(s1_terms[3], next_copy(x1, &ix1, FANOUT_PER_LIT),
        next_copy(x3, &ix3, FANOUT_PER_LIT));

    s1_terms[4] = scratch_alloc(&sp);
    AND(s1_terms[4], next_copy(nx2, &inx2, FANOUT_PER_LIT),
        next_copy(x3, &ix3, FANOUT_PER_LIT));

    or_reduce(out[1], s1_terms, 5, &sp);

    /* Segment 2 */
    void *s2_terms[5];
    s2_terms[0] = scratch_alloc(&sp);
    and3(s2_terms[0],
         next_copy(nx0, &inx0, FANOUT_PER_LIT),
         next_copy(nx1, &inx1, FANOUT_PER_LIT),
         next_copy(nx3, &inx3, FANOUT_PER_LIT),
         &sp);

    s2_terms[1] = scratch_alloc(&sp);
    AND(s2_terms[1], next_copy(nx0, &inx0, FANOUT_PER_LIT),
        next_copy(nx2, &inx2, FANOUT_PER_LIT));

    s2_terms[2] = scratch_alloc(&sp);
    and3(s2_terms[2],
         next_copy(x0, &ix0, FANOUT_PER_LIT),
         next_copy(nx1, &inx1, FANOUT_PER_LIT),
         next_copy(x3, &ix3, FANOUT_PER_LIT),
         &sp);

    s2_terms[3] = scratch_alloc(&sp);
    and3(s2_terms[3],
         next_copy(x0, &ix0, FANOUT_PER_LIT),
         next_copy(x1, &ix1, FANOUT_PER_LIT),
         next_copy(nx3, &inx3, FANOUT_PER_LIT),
         &sp);

    s2_terms[4] = scratch_alloc(&sp);
    AND(s2_terms[4], next_copy(nx1, &inx1, FANOUT_PER_LIT),
        next_copy(nx2, &inx2, FANOUT_PER_LIT));

    or_reduce(out[2], s2_terms, 5, &sp);

    /* Segment 3 */
    void *s3_terms[5];
    s3_terms[0] = scratch_alloc(&sp);
    AND(s3_terms[0], next_copy(nx0, &inx0, FANOUT_PER_LIT),
        next_copy(x1, &ix1, FANOUT_PER_LIT));

    s3_terms[1] = scratch_alloc(&sp);
    AND(s3_terms[1], next_copy(x0, &ix0, FANOUT_PER_LIT),
        next_copy(x3, &ix3, FANOUT_PER_LIT));

    s3_terms[2] = scratch_alloc(&sp);
    and3(s3_terms[2],
         next_copy(nx1, &inx1, FANOUT_PER_LIT),
         next_copy(x2, &ix2, FANOUT_PER_LIT),
         next_copy(nx3, &inx3, FANOUT_PER_LIT),
         &sp);

    s3_terms[3] = scratch_alloc(&sp);
    AND(s3_terms[3], next_copy(x1, &ix1, FANOUT_PER_LIT),
        next_copy(nx2, &inx2, FANOUT_PER_LIT));

    s3_terms[4] = scratch_alloc(&sp);
    AND(s3_terms[4], next_copy(nx2, &inx2, FANOUT_PER_LIT),
        next_copy(x3, &ix3, FANOUT_PER_LIT));

    or_reduce(out[3], s3_terms, 5, &sp);

    /* Segment 4 */
    void *s4_terms[4];
    s4_terms[0] = scratch_alloc(&sp);
    AND(s4_terms[0], next_copy(nx0, &inx0, FANOUT_PER_LIT),
        next_copy(x1, &ix1, FANOUT_PER_LIT));

    s4_terms[1] = scratch_alloc(&sp);
    AND(s4_terms[1], next_copy(nx0, &inx0, FANOUT_PER_LIT),
        next_copy(nx2, &inx2, FANOUT_PER_LIT));

    s4_terms[2] = scratch_alloc(&sp);
    AND(s4_terms[2], next_copy(x1, &ix1, FANOUT_PER_LIT),
        next_copy(x3, &ix3, FANOUT_PER_LIT));

    s4_terms[3] = scratch_alloc(&sp);
    AND(s4_terms[3], next_copy(x2, &ix2, FANOUT_PER_LIT),
        next_copy(x3, &ix3, FANOUT_PER_LIT));

    or_reduce(out[4], s4_terms, 4, &sp);

    /* Segment 5 */
    void *s5_terms[5];
    s5_terms[0] = scratch_alloc(&sp);
    AND(s5_terms[0], next_copy(x0, &ix0, FANOUT_PER_LIT),
        next_copy(nx1, &inx1, FANOUT_PER_LIT));

    s5_terms[1] = scratch_alloc(&sp);
    AND(s5_terms[1], next_copy(x0, &ix0, FANOUT_PER_LIT),
        next_copy(nx2, &inx2, FANOUT_PER_LIT));

    s5_terms[2] = scratch_alloc(&sp);
    AND(s5_terms[2], next_copy(nx1, &inx1, FANOUT_PER_LIT),
        next_copy(nx2, &inx2, FANOUT_PER_LIT));

    s5_terms[3] = scratch_alloc(&sp);
    AND(s5_terms[3], next_copy(nx2, &inx2, FANOUT_PER_LIT),
        next_copy(x3, &ix3, FANOUT_PER_LIT));

    s5_terms[4] = scratch_alloc(&sp);
    AND(s5_terms[4], next_copy(x2, &ix2, FANOUT_PER_LIT),
        next_copy(nx3, &inx3, FANOUT_PER_LIT));

    or_reduce(out[5], s5_terms, 5, &sp);

    /* Segment 6 */
    void *s6_terms[5];
    s6_terms[0] = scratch_alloc(&sp);
    and3(s6_terms[0],
         next_copy(nx0, &inx0, FANOUT_PER_LIT),
         next_copy(nx1, &inx1, FANOUT_PER_LIT),
         next_copy(nx2, &inx2, FANOUT_PER_LIT),
         &sp);

    s6_terms[1] = scratch_alloc(&sp);
    and3(s6_terms[1],
         next_copy(nx0, &inx0, FANOUT_PER_LIT),
         next_copy(x1, &ix1, FANOUT_PER_LIT),
         next_copy(nx3, &inx3, FANOUT_PER_LIT),
         &sp);

    s6_terms[2] = scratch_alloc(&sp);
    and3(s6_terms[2],
         next_copy(nx0, &inx0, FANOUT_PER_LIT),
         next_copy(x2, &ix2, FANOUT_PER_LIT),
         next_copy(x3, &ix3, FANOUT_PER_LIT),
         &sp);

    s6_terms[3] = scratch_alloc(&sp);
    and3(s6_terms[3],
         next_copy(x0, &ix0, FANOUT_PER_LIT),
         next_copy(nx1, &inx1, FANOUT_PER_LIT),
         next_copy(x2, &ix2, FANOUT_PER_LIT),
         &sp);

    s6_terms[4] = scratch_alloc(&sp);
    and3(s6_terms[4],
         next_copy(x0, &ix0, FANOUT_PER_LIT),
         next_copy(x1, &ix1, FANOUT_PER_LIT),
         next_copy(nx2, &inx2, FANOUT_PER_LIT),
         &sp);

    or_reduce(out[6], s6_terms, 5, &sp);
}
