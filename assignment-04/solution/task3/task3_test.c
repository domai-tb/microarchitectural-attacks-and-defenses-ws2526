#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "solution.h"

int main(int argc, char *argv[]) {
    int samples = 200;
    if (argc >= 2) {
        samples = atoi(argv[1]);
        if (samples <= 0) {
            fprintf(stderr, "Invalid samples value\n");
            return EXIT_FAILURE;
        }
    }

    uint8_t *buf = NULL;
    if (posix_memalign((void **)&buf, PAGE_SIZE, PAGE_SIZE) != 0) {
        fprintf(stderr, "posix_memalign failed\n");
        return EXIT_FAILURE;
    }
    memset(buf, 0, PAGE_SIZE);

    int correct = 0;
    int total = 0;

    for (int i = 0; i < samples; i++) {
        uint8_t expected = (uint8_t)((i * 29) & 0xff);
        buf[0] = expected;

        int observed = do_meltdown((uintptr_t)&buf[0]);
        printf("valid %u %d\n", expected, observed);

        if (observed == expected) {
            correct++;
        }
        total++;
    }

    fprintf(stderr, "Valid accuracy: %.2f%% (%d/%d)\n",
            100.0 * (double)correct / (double)total, correct, total);

    int invalid = do_meltdown(0);
    printf("invalid -1 %d\n", invalid);
    fprintf(stderr, "Invalid address observed: %d\n", invalid);

    free(buf);
    return EXIT_SUCCESS;
}
