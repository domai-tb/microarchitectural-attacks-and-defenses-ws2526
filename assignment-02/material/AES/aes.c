#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <assert.h>
#include <string.h>
#include "aes.h"


#define AESSIZE 16

typedef uint8_t aes_t[AESSIZE];


void tobinary(char *data, aes_t aes) {
  assert(strlen(data)==AESSIZE*2);
  unsigned int x;
  for (int i = 0; i < AESSIZE; i++) {
    sscanf(data+i*2, "%2x", &x);
    aes[i] = x;
  }
}

char *toString(aes_t aes) {
  char buf[AESSIZE * 2 + 1];
  for (int i = 0; i < AESSIZE; i++)
    sprintf(buf + i*2, "%02x", aes[i]);
  return strdup(buf);
}

void randaes(aes_t aes) {
  for (int i = 0; i < AESSIZE; i++)
    aes[i] = rand() & 0xff;
}


int main(int ac, char **av) {
  aes_t key, input, output;
  tobinary("00000000000000000000000000000000", key);
  AES_KEY aeskey;
  private_AES_set_encrypt_key(key, 128, &aeskey);

  tobinary("00000000000000000000000000000000", input);
  AES_encrypt(input, output, &aeskey);
  char *x = toString(output);
  printf("%s\n", x);
  free(x);
}
