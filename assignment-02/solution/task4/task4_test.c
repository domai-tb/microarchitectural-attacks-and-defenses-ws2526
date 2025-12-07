#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "solution.h"
#include "aes.h"

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

static void compute_true_k10(const uint8_t key[16], uint8_t out_k10[16]) {
    AES_KEY aeskey;
    if (private_AES_set_encrypt_key(key, 128, &aeskey) != 0) {
        fprintf(stderr, "private_AES_set_encrypt_key failed in test\n");
        memset(out_k10, 0, 16);
        return;
    }

    uint32_t *rk = aeskey.rd_key;
    for (int i = 0; i < 4; i++) {
        uint32_t w = rk[40 + i];
        out_k10[4 * i + 0] = (uint8_t)((w >> 24) & 0xff);
        out_k10[4 * i + 1] = (uint8_t)((w >> 16) & 0xff);
        out_k10[4 * i + 2] = (uint8_t)((w >> 8)  & 0xff);
        out_k10[4 * i + 3] = (uint8_t)( w        & 0xff);
    }
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

    uint8_t recovered_k10[16];
    task4_attack_last_round(samples, key, recovered_k10);

    uint8_t true_k10[16];
    compute_true_k10(key, true_k10);

    printf("True  K10: ");
    for (int i = 0; i < 16; i++) {
        printf("%02X", true_k10[i]);
    }
    printf("\n");

    printf("Guess K10: ");
    for (int i = 0; i < 16; i++) {
        printf("%02X", recovered_k10[i]);
    }
    printf("\n");

    int correct = 0;
    for (int i = 0; i < 16; i++) {
        if (recovered_k10[i] == true_k10[i]) {
            correct++;
        }
    }
    printf("Correct bytes: %d / 16\n", correct);

    return EXIT_SUCCESS;
}
