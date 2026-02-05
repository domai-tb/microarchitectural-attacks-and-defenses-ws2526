#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "solution.h"

#define TIMING_SAMPLES 1000

static void write_timing_csv(const char *path) {
    FILE *csv = fopen(path, "w");
    if (!csv) {
        fprintf(stderr, "Failed to open %s\n", path);
        return;
    }
    fprintf(csv, "type,cycles\n");

    uint8_t *probe = NULL;
    if (posix_memalign((void **)&probe, PAGE_SIZE, PAGE_SIZE) != 0) {
        fprintf(stderr, "posix_memalign failed for timing probe\n");
        fclose(csv);
        return;
    }
    memset(probe, 1, PAGE_SIZE);

    for (int i = 0; i < TIMING_SAMPLES; i++) {
        maccess(probe);
        uint64_t dt = timed_access(probe);
        fprintf(csv, "cached,%lu\n", dt);
    }

    for (int i = 0; i < TIMING_SAMPLES; i++) {
        clflush_line(probe);
        uint64_t dt = timed_access(probe);
        fprintf(csv, "uncached,%lu\n", dt);
    }

    free(probe);
    fclose(csv);
}

int main(int argc, char *argv[]) {
    int samples_per_value = 50;
    if (argc >= 2) {
        samples_per_value = atoi(argv[1]);
        if (samples_per_value <= 0) {
            fprintf(stderr, "Invalid samples value\n");
            return EXIT_FAILURE;
        }
    }

    cc_init();
    uint64_t threshold = cc_get_threshold();
    fprintf(stderr, "CC threshold: %lu cycles\n", threshold);

    write_timing_csv("task1/data/cc_timing.csv");

    int correct = 0;
    int total = 0;

    for (int v = 0; v < 256; v++) {
        for (int t = 0; t < samples_per_value; t++) {
            cc_setup();
            cc_transmit((uint8_t)v);
            int observed = cc_receive();

            printf("%d %d\n", v, observed);
            if (observed == v) {
                correct++;
            }
            total++;
        }
    }

    fprintf(stderr, "Accuracy: %.2f%% (%d/%d)\n",
            100.0 * (double)correct / (double)total, correct, total);

    return EXIT_SUCCESS;
}
