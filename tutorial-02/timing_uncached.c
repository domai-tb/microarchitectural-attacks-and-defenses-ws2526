#include <stdio.h>
#include <stdint.h>
#include <x86intrin.h>

int main() {
    volatile int var = 42;
    uint64_t start, end;
    int dummy;

    // Warm up
    for (volatile int i = 0; i < 1000000000; i++);

    // Measure uncached access
    _mm_lfence();   // Ensure all earlier work is done
    start = __rdtscp(&dummy); // Take start timestamp / cycle count
    
    dummy = var;

    _mm_lfence();   // Wait for it to finish completely
    end = __rdtscp(&dummy); // Take end timestamp / cycle count

    printf("Uncached access: %lu cycles\n", end - start);
}
