#include <stdio.h>
#include <stdint.h>
#include <x86intrin.h>



void cc_init() {
  // Implement
}

void cc_setup() {
  // Implement
}

void cc_transmit(uint8_t value) {
  // Implement
}

int cc_receive() {
  // Implement
}


void meltdown(uintptr_t adrs) {
  volatile int tmp = 0;
  cc_setup();
  _mm_lfence();
  tmp+=17;
  tmp*=59;
  tmp|=73;
  tmp/=123;
  asm volatile("":::"memory");
  uint8_t rv = *((uint8_t *)adrs);
  cc_transmit(rv);
}



int do_meltdown(uintptr_t adrs) {
  // Implement
}
