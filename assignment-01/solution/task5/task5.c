#include <stdint.h>
#include <stdio.h>
#include <time.h>
#include <x86intrin.h>

#include "solution.h"

void fr_trace(void *p1, void *p2, uint32_t period, uint32_t count, uint32_t *trace) {
    uint64_t start_time = __rdtsc(); 

    for (uint32_t i = 0; i < count; ++i) {
        
        // Check only after period cycles
        while ((__rdtsc() - start_time) < period) {
            // Sleep until next period is starting
        }

        // Measure the reload times for p1 and p2
        uint32_t cycles_p1 = flush_reload(p1);
        uint32_t cycles_p2 = flush_reload(p2);

        // Store the results in trace[2*i] and trace[2*i + 1]
        trace[2 * i] = cycles_p1;
        trace[2 * i + 1] = cycles_p2;
    }
}
