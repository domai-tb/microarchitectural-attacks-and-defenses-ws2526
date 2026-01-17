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

static void flush_outputs(void **outs, int n_out) {
    for (int i = 0; i < n_out; i++) {
        clflush_line(outs[i]);
    }
    asm volatile("mfence" ::: "memory");
}

static void eval_not_multi(void **outs, int max_out, void *in,
                           uint64_t threshold, int samples) {
    for (int n_out = 1; n_out <= max_out; n_out++) {
        int correct[max_out];
        int total[max_out];
        for (int i = 0; i < n_out; i++) {
            correct[i] = 0;
            total[i] = 0;
        }

        for (int input = 0; input <= 1; input++) {
            for (int t = 0; t < samples; t++) {
                set_cached(in, input);
                flush_outputs(outs, n_out);

                NOT_M(outs, n_out, in);

                int expected = input ? 0 : 1;
                for (int i = 0; i < n_out; i++) {
                    uint64_t dt = timed_access(outs[i]);
                    int observed = (dt < threshold) ? 1 : 0;
                    correct[i] += (expected == observed);
                    total[i]++;
                }
            }
        }

        for (int i = 0; i < n_out; i++) {
            double acc = 100.0 * (double)correct[i] / (double)total[i];
            printf("NOT_M %d %d %.2f\n", n_out, i, acc);
        }
    }
}

static void eval_bin_multi(const char *name,
                           void (*gate)(void **, int, void *, void *),
                           void **outs, int max_out,
                           void *in1, void *in2,
                           uint64_t threshold, int samples) {
    for (int n_out = 1; n_out <= max_out; n_out++) {
        int correct[max_out];
        int total[max_out];
        for (int i = 0; i < n_out; i++) {
            correct[i] = 0;
            total[i] = 0;
        }

        for (int a = 0; a <= 1; a++) {
            for (int b = 0; b <= 1; b++) {
                int expected = (name[0] == 'N' && name[1] == 'A') ? (!(a && b))
                              : (!(a || b));
                for (int t = 0; t < samples; t++) {
                    set_cached(in1, a);
                    set_cached(in2, b);
                    flush_outputs(outs, n_out);

                    gate(outs, n_out, in1, in2);

                    for (int i = 0; i < n_out; i++) {
                        uint64_t dt = timed_access(outs[i]);
                        int observed = (dt < threshold) ? 1 : 0;
                        correct[i] += (expected == observed);
                        total[i]++;
                    }
                }
            }
        }

        for (int i = 0; i < n_out; i++) {
            double acc = 100.0 * (double)correct[i] / (double)total[i];
            printf("%s %d %d %.2f\n", name, n_out, i, acc);
        }
    }
}

int main(int argc, char *argv[]) {
    int samples = 200;
    int max_out = 16;

    if (argc >= 2) {
        samples = atoi(argv[1]);
        if (samples <= 0) {
            fprintf(stderr, "Invalid samples value\n");
            return EXIT_FAILURE;
        }
    }
    if (argc >= 3) {
        max_out = atoi(argv[2]);
        if (max_out <= 0 || max_out > 64) {
            fprintf(stderr, "Invalid max_out value\n");
            return EXIT_FAILURE;
        }
    }

    size_t region_size = (size_t)(max_out + 3) * PAGE_SIZE;
    uint8_t *region = NULL;
    if (posix_memalign((void **)&region, PAGE_SIZE, region_size) != 0) {
        fprintf(stderr, "posix_memalign failed\n");
        return EXIT_FAILURE;
    }
    memset(region, 0, region_size);

    void *in1 = region;
    void *in2 = region + PAGE_SIZE;
    void *out_base = region + 2 * PAGE_SIZE;

    void **outs = calloc((size_t)max_out, sizeof(void *));
    if (!outs) {
        fprintf(stderr, "calloc failed\n");
        free(region);
        return EXIT_FAILURE;
    }
    for (int i = 0; i < max_out; i++) {
        outs[i] = (uint8_t *)out_base + (size_t)i * PAGE_SIZE;
    }

    uint64_t threshold = calibrate_threshold(outs[0]);
    fprintf(stderr, "Threshold: %lu cycles\n", threshold);

    eval_not_multi(outs, max_out, in1, threshold, samples);
    eval_bin_multi("NAND_M", NAND_M, outs, max_out, in1, in2, threshold, samples);
    eval_bin_multi("NOR_M", NOR_M, outs, max_out, in1, in2, threshold, samples);

    free(outs);
    free(region);
    return EXIT_SUCCESS;
}
