#include <stdio.h>
#include <stdint.h>
#include <x86intrin.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>

#define PAGE_SIZE 4096

int main() {
    char *anon = mmap(NULL, 4096, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    int allZero = 1;

    for (int i = 0; i < 4096; i++) {
        if (anon[i] != 0) { 
            printf("Not zero!\n"); 
            allZero = 0;
            break; 
        }
    }

    if (allZero) {
        printf("All zero!\n");
    }

    munmap(anon, 4096);

    return 0;
}
