#ifndef SOLUTION_H
#define SOLUTION_H

#include <stdint.h>
#include <stddef.h>
#include <x86intrin.h>

#define CACHE_LINE 64
#define PAGE_SIZE  4096

#ifndef GATE_TRAINING_ITERS
#define GATE_TRAINING_ITERS 50
#endif

#ifndef GATE_DELAY_ITERS
#define GATE_DELAY_ITERS 64
#endif

#ifndef GATE_WINDOW_DELAY_ITERS
#define GATE_WINDOW_DELAY_ITERS 80
#endif

#ifndef GATE_WINDOW_TRAIN_ITERS
#define GATE_WINDOW_TRAIN_ITERS 50
#endif

static inline uint64_t rdtsc64(void) {
    return __rdtsc();
}

static inline uint64_t rdtsc_begin(void) {
    unsigned hi, lo;
    asm volatile("mfence\n\tlfence\n\trdtsc\n\t"
                 "mov %%eax, %0\n\t"
                 "mov %%edx, %1\n\t"
                 "lfence"
                 : "=r"(lo), "=r"(hi)
                 :
                 : "%rax", "%rdx", "memory");
    return ((uint64_t)hi << 32) | lo;
}

static inline uint64_t rdtsc_end(void) {
    unsigned hi, lo;
    asm volatile("lfence\n\trdtsc\n\t"
                 "mov %%eax, %0\n\t"
                 "mov %%edx, %1\n\t"
                 : "=r"(lo), "=r"(hi)
                 :
                 : "%rax", "%rdx", "memory");
    return ((uint64_t)hi << 32) | lo;
}

static inline void clflush_line(void *p) {
    asm volatile("clflush 0(%0)" :: "r"(p) : "memory");
}

static inline void maccess(void *p) {
    asm volatile("movb (%0), %%al\n" :: "r"(p) : "rax", "memory");
}

static inline uint64_t timed_access(void *p) {
    uint64_t start = rdtsc_begin();
    maccess(p);
    uint64_t end = rdtsc_end();
    return end - start;
}

void NOT(void *out, void *in);
void NOR(void *out, void *in1, void *in2);
void NAND(void *out, void *in1, void *in2);

void NOT_M(void **outs, int n_out, void *in);
void NOR_M(void **outs, int n_out, void *in1, void *in2);
void NAND_M(void **outs, int n_out, void *in1, void *in2);

void AND(void *out, void *in1, void *in2);
void OR(void *out, void *in1, void *in2);

void XOR(void *out, void *in1, void *in2);

void seven_segment(void **out, void **in);

#endif
