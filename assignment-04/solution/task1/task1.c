#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "solution.h"

#define CALIBRATION_SAMPLES 1000

static uint8_t *g_cc_pages = NULL;
static uint64_t g_cc_threshold = 0;
static uint8_t g_cc_order[CC_PAGES];

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

void cc_init(void) {
    if (g_cc_pages) {
        return;
    }

    size_t size = (size_t)CC_PAGES * PAGE_SIZE;
    if (posix_memalign((void **)&g_cc_pages, PAGE_SIZE, size) != 0) {
        fprintf(stderr, "posix_memalign failed in cc_init\n");
        exit(EXIT_FAILURE);
    }
    memset(g_cc_pages, 1, size);

    for (int i = 0; i < CC_PAGES; i++) {
        g_cc_order[i] = (uint8_t)((i * 167 + 13) & 0xff);
    }

    g_cc_threshold = calibrate_threshold(g_cc_pages);
}

void cc_setup(void) {
    if (!g_cc_pages) {
        cc_init();
    }

    for (int i = 0; i < CC_PAGES; i++) {
        clflush_line(g_cc_pages + ((size_t)i * PAGE_SIZE));
    }
    asm volatile("mfence" ::: "memory");
}

void cc_transmit(uint8_t value) {
    if (!g_cc_pages) {
        cc_init();
    }

    maccess(g_cc_pages + ((size_t)value * PAGE_SIZE));
}

int cc_receive(void) {
    if (!g_cc_pages) {
        cc_init();
    }

    uint64_t best_time = (uint64_t)-1;
    int best_idx = -1;

    for (int i = 0; i < CC_PAGES; i++) {
        int idx = g_cc_order[i];
        uint8_t *p = g_cc_pages + ((size_t)idx * PAGE_SIZE);
        uint64_t dt = timed_access(p);
        if (dt < best_time) {
            best_time = dt;
            best_idx = idx;
        }
    }

    if (best_time < g_cc_threshold) {
        return best_idx;
    }
    return -1;
}

uint64_t cc_get_threshold(void) {
    return g_cc_threshold;
}
