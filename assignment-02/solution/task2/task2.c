#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <unistd.h>

#include "solution.h"

typedef struct node {
    struct node *next;
    uint8_t      padding[56]; /* ensure one node per 64-byte line */
} node_t;

static uint8_t *g_region      = NULL;
static size_t   g_region_size = 0;
static node_t  *g_sets[L1_SETS];

static inline int l1_set_from_addr(void *addr) {
    uintptr_t a = (uintptr_t)addr;
    return (int)((a >> 6) & 0x3f);
}

int task2_init(size_t region_size) {
    if (g_region != NULL) {
        return 0;
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

    node_t *tmp[L1_SETS][L1_ASSOC];
    int counts[L1_SETS] = {0};

    for (size_t off = 0; off + L1_LINE <= g_region_size; off += L1_LINE) {
        node_t *n = (node_t *)(g_region + off);
        int set = l1_set_from_addr(n);
        if (set < 0 || set >= L1_SETS) {
            continue;
        }
        if (counts[set] < L1_ASSOC) {
            tmp[set][counts[set]++] = n;
        }
    }

    for (int s = 0; s < L1_SETS; s++) {
        if (counts[s] < L1_ASSOC) {
            fprintf(stderr, "Not enough lines for set %d (have %d, need %d)\n",
                    s, counts[s], L1_ASSOC);
            return -1;
        }
        for (int i = 0; i < L1_ASSOC; i++) {
            tmp[s][i]->next = tmp[s][(i + 1) % L1_ASSOC];
        }
        g_sets[s] = tmp[s][0];
    }

    return 0;
}

void task2_prime_all(void) {
    for (int s = 0; s < L1_SETS; s++) {
        volatile node_t *p = g_sets[s];
        for (int i = 0; i < L1_ASSOC; i++) {
            p = p->next;
        }
    }
}

void task2_rattle_cache_sets(int n, const int *sets, uint32_t iterations) {
    volatile node_t *p;
    for (uint32_t it = 0; it < iterations; it++) {
        for (int i = 0; i < n; i++) {
            int s = sets[i];
            if (s < 0 || s >= L1_SETS) {
                continue;
            }
            p = g_sets[s];
            for (int k = 0; k < L1_ASSOC; k++) {
                p = p->next;
            }
        }
    }
}

uint64_t task2_probe_set(int set_index) {
    if (set_index < 0 || set_index >= L1_SETS) {
        return 0;
    }

    volatile node_t *p = g_sets[set_index];
    uint64_t start, end;

    /* simple serialization */
    asm volatile("lfence" ::: "memory");
    start = rdtsc64();
    for (int i = 0; i < L1_ASSOC; i++) {
        p = p->next;
    }
    end = rdtsc64();
    asm volatile("lfence" ::: "memory");

    return end - start;
}

void task2_cleanup(void) {
    if (g_region) {
        munmap(g_region, g_region_size);
        g_region = NULL;
        g_region_size = 0;
    }
}
