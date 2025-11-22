#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#include <solution.h>

uint32_t flush_reload(void *p) {
    uint32_t cycles;

    // Cast pointer to address
    volatile unsigned char *addr = (volatile unsigned char*)p;

    asm volatile (
        // Fence previous instructions and memory loads
        "mfence\n"
        "lfence\n"
        // Messure cycles
        "rdtsc\n"
        // Save low 32 bits of start in edi
        "movl %%eax, %%edi\n"
        // Read the byte at addr into al (this loads it into cache)
        "movb (%1), %%al\n"
        // Fence  memory load
        "lfence\n"
        // Mesure cycles again
        "rdtsc\n"
        // Calculate operation cycles
        "subl %%edi, %%eax\n"
        // Flush
        "clflush 0(%1)\n"
        : "=a" (cycles)
        : "c" (addr)
        : "%edi", "%edx", "memory"
    );

    return cycles;
}