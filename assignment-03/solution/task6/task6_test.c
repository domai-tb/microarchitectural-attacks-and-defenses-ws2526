#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "solution.h"

#define CALIBRATION_SAMPLES 1000

static const int expected_table[16][7] = {
    {1, 1, 1, 0, 1, 1, 1}, /* 0x0 */
    {0, 0, 1, 0, 0, 1, 0}, /* 0x1 */
    {1, 0, 1, 1, 1, 0, 1}, /* 0x2 */
    {1, 0, 1, 1, 0, 1, 1}, /* 0x3 */
    {0, 1, 1, 1, 0, 1, 0}, /* 0x4 */
    {1, 1, 0, 1, 0, 1, 1}, /* 0x5 */
    {1, 1, 0, 1, 1, 1, 1}, /* 0x6 */
    {1, 0, 1, 0, 0, 1, 0}, /* 0x7 */
    {1, 1, 1, 1, 1, 1, 1}, /* 0x8 */
    {1, 1, 1, 1, 0, 1, 0}, /* 0x9 */
    {1, 1, 1, 1, 1, 1, 0}, /* 0xa */
    {0, 1, 0, 1, 1, 1, 1}, /* 0xb */
    {1, 1, 0, 0, 1, 0, 1}, /* 0xc */
    {0, 0, 1, 1, 1, 1, 1}, /* 0xd */
    {1, 1, 0, 1, 1, 0, 1}, /* 0xe */
    {1, 1, 0, 1, 1, 0, 0}, /* 0xf */
};

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

static void flush_outputs(void **outs, int n) {
    for (int i = 0; i < n; i++) {
        clflush_line(outs[i]);
    }
    asm volatile("mfence" ::: "memory");
}

int main(int argc, char *argv[]) {
    int samples = 200;
    if (argc >= 2) {
        samples = atoi(argv[1]);
        if (samples <= 0) {
            fprintf(stderr, "Invalid samples value\n");
            return EXIT_FAILURE;
        }
    }

    uint8_t *region = NULL;
    if (posix_memalign((void **)&region, PAGE_SIZE, PAGE_SIZE * 16) != 0) {
        fprintf(stderr, "posix_memalign failed\n");
        return EXIT_FAILURE;
    }
    memset(region, 0, PAGE_SIZE * 16);

    void *in[4];
    void *out[7];
    for (int i = 0; i < 4; i++) {
        in[i] = region + (size_t)i * PAGE_SIZE;
    }
    for (int i = 0; i < 7; i++) {
        out[i] = region + (size_t)(4 + i) * PAGE_SIZE;
    }

    uint64_t threshold = calibrate_threshold(out[0]);
    fprintf(stderr, "Threshold: %lu cycles\n", threshold);

    int correct = 0;
    int total = 0;

    for (int value = 0; value < 16; value++) {
        int b0 = (value >> 0) & 1;
        int b1 = (value >> 1) & 1;
        int b2 = (value >> 2) & 1;
        int b3 = (value >> 3) & 1;

        for (int t = 0; t < samples; t++) {
            set_cached(in[0], b0);
            set_cached(in[1], b1);
            set_cached(in[2], b2);
            set_cached(in[3], b3);

            flush_outputs(out, 7);

            seven_segment(out, in);

            for (int seg = 0; seg < 7; seg++) {
                uint64_t dt = timed_access(out[seg]);
                int observed = (dt < threshold) ? 1 : 0;
                int expected = expected_table[value][seg];
                printf("%d %d %d %d\n", value, seg, expected, observed);
                correct += (expected == observed);
                total++;
            }
        }
    }

    fprintf(stderr, "7-seg accuracy: %.2f%% (%d/%d)\n",
            100.0 * (double)correct / (double)total, correct, total);

    free(region);
    return EXIT_SUCCESS;
}
