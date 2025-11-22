#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <x86intrin.h>

#include "solution.h"

#define MAX_EVENTS 2000
#define TIMEOUT 10000000000ULL   // 10 billion cycles

int main(int argc, char *argv[]) {
    if (argc != 4) {
        fprintf(stderr, "Usage: %s <filename> <offset> <period>\n", argv[0]);
        return EXIT_FAILURE;
    }

    char *filename = argv[1];
    uint64_t offset = strtoull(argv[2], NULL, 10);
    uint32_t period = (uint32_t)strtoul(argv[3], NULL, 10);

    // Map the file
    void *ptr = map(filename, offset);
    if (!ptr) {
        fprintf(stderr, "Failed to map file %s\n", filename);
        return EXIT_FAILURE;
    }

    // Array to store timestamps of detected accesses
    uint64_t hits[MAX_EVENTS];
    size_t count = 0;

    // Start timestamp
    uint64_t start_tsc = __rdtsc();

    while (count < MAX_EVENTS) {
        // Check if time ran out
        uint64_t now = __rdtsc();
        if (now - start_tsc > TIMEOUT) {
            break;
        }

        // Use monitor from task3
        int detected = monitor(ptr, period, period);

        if (detected == 1) {
            // Store timestamp
            hits[count++] = __rdtsc();
        }
    }

    // Print results at end
    for (size_t i = 0; i < count; i++) {
        printf("%lu\n", hits[i]);
    }

    return EXIT_SUCCESS;
}
