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

static void run_gate(const char *name,
                     void (*gate)(void *, void *, void *),
                     void *out,
                     void *in1,
                     void *in2,
                     uint64_t threshold,
                     int samples) {
    int correct = 0;
    int total = 0;

    for (int a = 0; a <= 1; a++) {
        for (int b = 0; b <= 1; b++) {
            int expected = (name[0] == 'A') ? (a && b) : (a || b);
            for (int t = 0; t < samples; t++) {
                set_cached(in1, a);
                set_cached(in2, b);
                clflush_line(out);
                asm volatile("mfence" ::: "memory");

                gate(out, in1, in2);

                uint64_t dt = timed_access(out);
                int observed = (dt < threshold) ? 1 : 0;

                printf("%s %d %d %d %d\n", name, a, b, expected, observed);

                correct += (expected == observed);
                total++;
            }
        }
    }

    fprintf(stderr, "%s accuracy: %.2f%% (%d/%d)\n",
            name, 100.0 * (double)correct / (double)total, correct, total);
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

    fprintf(stderr, "GATE_WINDOW_DELAY_ITERS=%d\n", GATE_WINDOW_DELAY_ITERS);

    uint8_t *region = NULL;
    if (posix_memalign((void **)&region, PAGE_SIZE, PAGE_SIZE * 3) != 0) {
        fprintf(stderr, "posix_memalign failed\n");
        return EXIT_FAILURE;
    }
    memset(region, 0, PAGE_SIZE * 3);

    void *in1 = region;
    void *in2 = region + PAGE_SIZE;
    void *out = region + 2 * PAGE_SIZE;

    uint64_t threshold = calibrate_threshold(out);
    fprintf(stderr, "Threshold: %lu cycles\n", threshold);

    run_gate("AND", AND, out, in1, in2, threshold, samples);
    run_gate("OR", OR, out, in1, in2, threshold, samples);

    free(region);
    return EXIT_SUCCESS;
}
