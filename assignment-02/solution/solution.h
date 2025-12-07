#ifndef SOLUTION_H
#define SOLUTION_H

#include <stdint.h>
#include <stddef.h>

#define L1_SETS   64
#define L1_LINE   64
#define L1_ASSOC  8

#include <x86intrin.h>
static inline uint64_t rdtsc64(void) {
    return __rdtsc();
}

int  task1_init(size_t region_size);
void task1_rattle_cache_sets(int n, const int *sets, uint32_t iterations);
void task1_cleanup(void);

int      task2_init(size_t region_size);
void     task2_prime_all(void);
void     task2_rattle_cache_sets(int n, const int *sets, uint32_t iterations);
uint64_t task2_probe_set(int set_index);
void     task2_cleanup(void);

void task3_attack_first_round(int samples, const uint8_t key[16],uint8_t out_upper_nibbles[16]);
void task4_attack_last_round(int samples, const uint8_t key[16],uint8_t out_k10[16]);

#endif
