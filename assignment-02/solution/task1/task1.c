#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <unistd.h>

#include "solution.h"

typedef struct {
    uint8_t *addrs[1024];
    size_t   count;
} set_bucket_t;

static uint8_t      *g_region        = NULL;
static size_t        g_region_size   = 0;
static set_bucket_t  g_buckets[L1_SETS];

static inline int l1_set_from_addr(void *addr) {
    uintptr_t a = (uintptr_t)addr;
    return (int)((a >> 6) & 0x3f); /* 64-byte lines, 64 sets */
}

static void build_buckets(void) {
    for (int s = 0; s < L1_SETS; s++) {
        g_buckets[s].count = 0;
    }

    for (size_t off = 0; off + L1_LINE <= g_region_size; off += L1_LINE) {
        uint8_t *addr = g_region + off;
        int set = l1_set_from_addr(addr);
        set_bucket_t *b = &g_buckets[set];
        if (b->count < (sizeof b->addrs / sizeof b->addrs[0])) {
            b->addrs[b->count++] = addr;
        }
    }
}

int task1_init(size_t region_size) {
    if (g_region != NULL) {
        return 0; /* already initialized */
    }

    g_region_size = region_size;
    g_region = mmap(NULL, g_region_size,
                    PROT_READ | PROT_WRITE,
                    MAP_PRIVATE | MAP_ANONYMOUS,
                    -1, 0);
    if (g_region == MAP_FAILED) {
        perror("mmap");
        g_region = NULL;
        return -1;
    }

    build_buckets();
    return 0;
}

void task1_rattle_cache_sets(int n, const int *sets, uint32_t iterations) {
    volatile uint8_t sink = 0;

    for (uint32_t it = 0; it < iterations; it++) {
        for (int i = 0; i < n; i++) {
            int s = sets[i];
            if (s < 0 || s >= L1_SETS) {
                continue;
            }
            set_bucket_t *b = &g_buckets[s];
            for (size_t j = 0; j < b->count; j++) {
                sink ^= *b->addrs[j];
            }
        }
    }

    (void)sink;
}

void task1_cleanup(void) {
    if (g_region) {
        munmap(g_region, g_region_size);
        g_region = NULL;
        g_region_size = 0;
    }
}
