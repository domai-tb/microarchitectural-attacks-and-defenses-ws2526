#include <stdio.h>
#include <stdint.h>
#include <x86intrin.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>

#define PAGE_SIZE 4096

// measure access time to a memory address
static inline uint64_t measure_access_time(volatile char *addr) {
    uint32_t aux;
    uint64_t start, end;
    _mm_lfence();
    start = __rdtscp(&aux);
    (void)*addr; // read the byte
    _mm_lfence();
    end = __rdtscp(&aux);
    return end - start;
}

int main() {
    // open file
    int fd = open("textfile.txt", O_RDONLY);
    if (fd < 0) {
        perror("open");
        return 1;
    }

    // map twice
    char *map1 = mmap(NULL, PAGE_SIZE, PROT_READ, MAP_PRIVATE, fd, 0);
    char *map2 = mmap(NULL, PAGE_SIZE, PROT_READ, MAP_PRIVATE, fd, 0);
    if (map1 == MAP_FAILED || map2 == MAP_FAILED) {
        perror("mmap");
        return 1;
    }

    // pick an offset inside the page (aligned to avoid prefetch effects)
    volatile char *addr1 = map1;
    volatile char *addr2 = map2;

    // warm up CPU frequency
    for (volatile int i = 0; i < 100000000; i++);

    // 1. Ensure both are out of cache
    asm volatile("clflush 0(%0)" :: "r"(addr1));
    asm volatile("clflush 0(%0)" :: "r"(addr2));
    _mm_mfence();

    // 2. Measure time of first access (cold)
    uint64_t t1 = measure_access_time(addr1);

    // 3. Measure time of second access (possibly cached)
    uint64_t t2 = measure_access_time(addr2);

    printf("Access time map1: %lu cycles\n", t1);
    printf("Access time map2: %lu cycles\n", t2);

    // If second access is more than half the time of the 
    // first access, than it was probaley cached. That means 
    // is used the same shared memory.
    if (t2 < t1 * 0.5)
        printf("Likely shared physical memory (cache hit)\n");
    else
        printf("Likely separate physical pages (cache miss)\n");

    munmap((void*)map1, PAGE_SIZE);
    munmap((void*)map2, PAGE_SIZE);
    close(fd);
    return 0;
}
