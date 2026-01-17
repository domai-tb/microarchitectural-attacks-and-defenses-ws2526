#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "solution.h"

#define CALIBRATION_SAMPLES 1000

static uint64_t calibrate_threshold(void *p) {
    uint64_t hit_sum = 0;
    uint64_t miss_sum = 0;

    for (int i = 0; i < CALIBRATION_SAMPLES; i++) {
        maccess(p);
        hit_sum += timed_access(p);

        clflush_line(p);
        miss_sum += timed_access(p);
    }

    uint64_t hit_avg = hit_sum / CALIBRATION_SAMPLES;
    uint64_t miss_avg = miss_sum / CALIBRATION_SAMPLES;
    return (hit_avg + miss_avg) / 2;
}

static void set_cached(void *p, int cached) {
    if (cached) {
        maccess(p);
    } else {
        clflush_line(p);
    }
    asm volatile("mfence" ::: "memory");
}

int main(int argc, char *argv[]) {
    int samples = 1000;
    if (argc >= 2) {
        samples = atoi(argv[1]);
        if (samples <= 0) {
            fprintf(stderr, "Invalid samples value\n");
            return EXIT_FAILURE;
        }
    }

    uint8_t *region = NULL;
    if (posix_memalign((void **)&region, PAGE_SIZE, PAGE_SIZE * 2) != 0) {
        fprintf(stderr, "posix_memalign failed\n");
        return EXIT_FAILURE;
    }
    memset(region, 0, PAGE_SIZE * 2);

    void *in = region;
    void *out = region + PAGE_SIZE;

    uint64_t threshold = calibrate_threshold(out);
    fprintf(stderr, "Threshold: %lu cycles\n", threshold);

    int correct = 0;
    int total = 0;

    for (int input = 0; input <= 1; input++) {
        for (int t = 0; t < samples; t++) {
            set_cached(in, input);
            clflush_line(out);
            asm volatile("mfence" ::: "memory");

            NOT(out, in);

            uint64_t dt = timed_access(out);
            int observed = (dt < threshold) ? 1 : 0;
            int expected = input ? 0 : 1;

            printf("%d %d %d\n", input, expected, observed);

            correct += (expected == observed);
            total++;
        }
    }

    fprintf(stderr, "Accuracy: %.2f%% (%d/%d)\n",
            100.0 * (double)correct / (double)total, correct, total);

    free(region);
    return EXIT_SUCCESS;
}
