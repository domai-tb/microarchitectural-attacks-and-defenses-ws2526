#ifndef SOLUTION_H
#define SOLUTION_H

#include <stdint.h>
#include <stddef.h>
#include <x86intrin.h>

#define CACHE_LINE 64
#define PAGE_SIZE  4096
#define CC_PAGES   256

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

void cc_init(void);
void cc_setup(void);
void cc_transmit(uint8_t value);
int  cc_receive(void);
uint64_t cc_get_threshold(void);

void meltdown(uintptr_t adrs);
int do_meltdown(uintptr_t adrs);

#endif
