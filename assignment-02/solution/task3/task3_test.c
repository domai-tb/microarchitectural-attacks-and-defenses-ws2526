#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "solution.h"

static int parse_hex_key(const char *hex, uint8_t *out_key) {
    size_t len = strlen(hex);
    if (len != 32) {
        return -1;
    }
    for (size_t i = 0; i < 16; i++) {
        char buf[3];
        buf[0] = hex[2 * i];
        buf[1] = hex[2 * i + 1];
        buf[2] = '\0';
        out_key[i] = (uint8_t)strtoul(buf, NULL, 16);
    }
    return 0;
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Usage: %s <samples> <hexkey>\n", argv[0]);
        return EXIT_FAILURE;
    }

    int samples = atoi(argv[1]);
    if (samples <= 0) {
        printf("Invalid samples value\n");
        return EXIT_FAILURE;
    }

    uint8_t key[16];
    if (parse_hex_key(argv[2], key) != 0) {
        printf("Invalid key format, expected 32 hex characters\n");
        return EXIT_FAILURE;
    }

    uint8_t upper_nibbles[16];
    task3_attack_first_round(samples, key, upper_nibbles);

    printf("Recovered upper nibbles (hex digits):\n");
    for (int i = 0; i < 16; i++) {
        printf("%X", upper_nibbles[i] & 0xF);
    }
    printf("\n");

    return EXIT_SUCCESS;
}
