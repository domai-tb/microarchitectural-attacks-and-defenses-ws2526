#include <stdio.h>
#include <stdint.h>
#include <x86intrin.h>

int main() {
    volatile int var = 42;
    uint64_t start, end;
    int dummy;

    // Warm up
    for (volatile int i = 0; i < 1000000000; i++);

    // Access once to ensure it's cached
    dummy = var;

    // Measure cached access
    _mm_lfence();
    start = __rdtscp(&dummy);
    dummy = var;
    _mm_lfence();
    end = __rdtscp(&dummy);

    printf("Cached access: %lu cycles\n", end - start);

    // Flush cache for variable
    asm volatile("clflush 0(%0)" :: "r"(&var));

    // Measure uncached access
    _mm_lfence();
    start = __rdtscp(&dummy);
    dummy = var;
    _mm_lfence();
    end = __rdtscp(&dummy);

    printf("Uncached access: %lu cycles\n", end - start);
}
