#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <string.h>

#include "solution.h"

#define PERIOD 10000
#define COUNT 1000

int main(int argc, char *argv[]) {
    if (argc != 4) {
        printf("Usage: %s <file> <offset1> <offset2>\n", argv[0]);
        return 1;
    }

    char *file_name = argv[1];
    uint64_t offset1 = strtoull(argv[2], NULL, 10);
    uint64_t offset2 = strtoull(argv[3], NULL, 10);

    // Map the two offsets into memory
    uint32_t *trace = (uint32_t *)malloc(2 * COUNT * sizeof(uint32_t)); 
    if (!trace) {
        printf("Memory allocation failed!\n");
        return EXIT_FAILURE;
    }

    // Map offsets
    void *p1 = map(file_name, offset1);
    void *p2 = map(file_name, offset2);

    if (!p1 || !p2) {
        printf("Mapping failed!\n");
        free(trace);
        return EXIT_FAILURE;
    }

    // Perform the trace
    fr_trace(p1, p2, PERIOD, COUNT, trace);

    // Save the result to a file
    FILE *out = fopen("trace.out", "w");
    if (!out) {
        printf("Failed to open output file\n");
        free(trace);
        return EXIT_FAILURE;
    }

    for (uint32_t i = 0; i < COUNT; i++) {
        fprintf(out, "%u %u\n", trace[2*i], trace[2*i + 1]);
    }

    fclose(out);
    free(trace); 

    return EXIT_SUCCESS;
}
