#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#include "solution.h"

int main(int argc, char* argv[]) {
    uint32_t period = 100; 
    uint64_t maxwait = UINT64_MAX;
    
    // Check CLI arguments
    if (argc != 2) {
        printf("Usage: %s <filepath>\n", argv[0]);
        return EXIT_FAILURE;
    }

    void *p = map(argv[1], 0);
    if (!p) {
        printf("Failed to map the file: %s\n", argv[1]);
        return EXIT_FAILURE;
    }

    int result = monitor(p, period, maxwait);
    if (result == 1) {
        printf("Memory accessed within the given time!\n");
    } else {
        printf("No memory access detected within the given time.\n");
    }

    return EXIT_SUCCESS;
}
