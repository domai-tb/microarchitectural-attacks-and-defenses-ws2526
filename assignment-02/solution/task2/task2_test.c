#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "solution.h"

#define REGION_SIZE (4 * 1024 * 1024)

int main(int argc, char *argv[]) {
    if (argc != 12) {
        printf("Usage: %s <samples> <set1> ... <set10>\n", argv[0]);
        return EXIT_FAILURE;
    }

    int samples = atoi(argv[1]);
    if (samples <= 0) {
        printf("Invalid samples value\n");
        return EXIT_FAILURE;
    }

    int rattled[10];
    for (int i = 0; i < 10; i++) {
        rattled[i] = atoi(argv[2 + i]);
    }

    if (task2_init(REGION_SIZE) != 0) {
        printf("task2_init failed\n");
        return EXIT_FAILURE;
    }

    for (int sample = 0; sample < samples; sample++) {
        task2_prime_all();
        task2_rattle_cache_sets(10, rattled, 10000);

        for (int s = 0; s < L1_SETS; s++) {
            uint64_t t = task2_probe_set(s);
            printf("%d %d %lu\n", sample, s, t);
        }
    }

    task2_cleanup();
    return EXIT_SUCCESS;
}
