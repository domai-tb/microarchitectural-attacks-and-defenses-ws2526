#include <stdint.h>
#include <stdio.h>
#include <time.h>
#include <x86intrin.h>

#include "solution.h"

int monitor(void* p, uint32_t period, uint64_t maxwait) {
    uint64_t start_time = __rdtsc();
    uint64_t elapsed_time = 0;

    while (elapsed_time < maxwait) {
        // Check only after period cycles
        while ((__rdtsc() - start_time) < period) {
            // Sleep until next period is starting
        }

        // Use flush+reload of task 2
        uint32_t cycles = flush_reload(p);

        // Threshold comes from task2 measurement where the maximum
        // cached cycle count was 144. See solution.md, section "Task 2"
        if (cycles < 150) {
            // Memory area was cached before
            return 1;
        }

        // Update time
        elapsed_time = __rdtsc() - start_time;
    }

    // No access detected in given maxwait time
    return 0;
}