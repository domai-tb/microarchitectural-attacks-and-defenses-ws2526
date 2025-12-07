#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "solution.h"

#define REGION_SIZE (4 * 1024 * 1024)

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Usage: %s <n> <set1> ... <setn>\n", argv[0]);
        return EXIT_FAILURE;
    }

    int n = atoi(argv[1]);
    if (n <= 0 || argc != 2 + n) {
        printf("Usage: %s <n> <set1> ... <setn>\n", argv[0]);
        return EXIT_FAILURE;
    }

    int *sets = (int *)malloc(n * sizeof(int));
    if (!sets) {
        printf("Failed to allocate memory for sets\n");
        return EXIT_FAILURE;
    }

    for (int i = 0; i < n; i++) {
        sets[i] = atoi(argv[2 + i]);
    }

    if (task1_init(REGION_SIZE) != 0) {
        printf("task1_init failed\n");
        free(sets);
        return EXIT_FAILURE;
    }

    printf("Rattling %d cache sets...\n", n);
    task1_rattle_cache_sets(n, sets, 10000);
    printf("Done.\n");

    task1_cleanup();
    free(sets);

    return EXIT_SUCCESS;
}
