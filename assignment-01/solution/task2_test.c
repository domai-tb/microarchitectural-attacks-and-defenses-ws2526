#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <time.h>

#include <solution.h>

// How many measurements?
#define SAMPLES 1 << 15

int main(int argc, char* argv[]) {
    // Check CLI arguments
    if (argc != 2) {
        printf("Usage: %s <filepath>\n", argv[0]);
        return EXIT_FAILURE;
    }

    void *ptr = map(argv[1], 0);
    if (!ptr) {
        printf("Mapping failed!\n");
        return EXIT_FAILURE;
    }

    // Output measurements as CSV to later analyze and plot with Python
    FILE *csv = fopen("task2.csv", "w");
    if (!csv) {
        printf("Failed to open CSV file");
        return EXIT_FAILURE;
    }
    fprintf(csv, "type,cycles\n");

    // Warm-up: access once to populate caches
    volatile unsigned char tmp = *(volatile unsigned char*)ptr;
    (void)tmp;

    // Measure Cached
    for (int i = 0; i < SAMPLES; i++) {
        // Load value to cache
        volatile unsigned char val = *(volatile unsigned char*)ptr;
        (void)val;

        // Measure cylces
        int32_t cycles = flush_reload(ptr);
        fprintf(csv, "cached,%u\n", cycles);
    }

    // Measure Uncached
    for (int i = 0; i < SAMPLES; i++) {
        asm volatile ("clflush 0(%0)" :: "r"(ptr) : "memory");
        uint32_t cycles = flush_reload(ptr);
        fprintf(csv, "uncached,%u\n", cycles);
    }

    fclose(csv);
    printf("Wrote task2.csv with %d samples.\n", SAMPLES);
    
    return EXIT_SUCCESS;
}