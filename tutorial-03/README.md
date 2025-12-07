# Tutorial 3 - Modelling L1 Cache

## Task 1 - Determine overall cache size

**Please read the whole task description before you start trying things.**

First, we will find out how large the L1 cache is. We can assume it to be a power of 2.

If we repeatedly access a single memory address and do nothing else, then we expect this memory will be loaded into the cache and not evicted. So, given there will almost always be a cache-hit, its average access time (across many multiple accesses) should be low. If we double the set of addresses accessed to two, then we expect the same behaviour.

If we continue doubling the size of the set of addresses that we access, at some point the set will reach double the size of the cache. At this point we expect half of our memory accesses to be cache-hits and half to be cache-misses, since memory accesses will evict one another approximately half of the time. Half-misses should result in a higher average access time.

Note we should ensure two things in our access pattern:

1. the overall number of accesses we perform is several multiples of the number of elements we access
2. the access pattern should be random within our overall set, this is to avoid any effects like data prefetching or the replacement policy having an influence on our measurements

Below is a function for generating fresh (pseudo-)randomness with a small, fixed number of instructions:

```c
#define FEEDBACK 0x80000000000019E2ULL
uint64_t step(uint64_t lfsr) {
lfsr = (lfsr & 1) ? (lfsr >> 1) ^ FEEDBACK : (lfsr >> 1);
return lfsr;
}
```

Before calling this function you need to initialise a random seed, you can use the following to do so:

```c
uint64_t lfsr;
asm volatile("rdrand %0": "=r" (lfsr)::"flags");
```

To accurately measure timing difference we want to make sure that in our access loop the following operations happen in order:

_random generation -> access random address -> random generation -> access random address -> ..._

To guarantee this we suggest incorporating dependency between each of these operations, e.g. each random generation takes a combination of the previous random value and the value accessed from the random address as input.

Your task is to use access times observed in this manner to determine the size of the L1 cache.

## Task 2 - Find the cache dimensions

Recall that the cache is organised into S cache sets that each contain W no. of ways. Memory lines each map to a certain cache set and can be stored in any of the ways within the set that they map to. Multiple lines map to the same set. Therefore the total cache size (in no. of lines) = S x W. From Task 1 we know the total cache size and now we want to find these other two parameters, when we find one of them we also find the other.

We begin with a set of memory addresses whose size is double the cache size. From the last task we know that if we access every element, the average access time is high (roughly halfway between access times for a cache-hit vs a cache-miss). If we then limit the access to only every second element within the same overall set, then the same timing should be observed. If we continue doubling our guessed stride length (i.e. taking every 2n th element), when we reach a guess equal to double the cache stride, the entire access set will fit in the cache and we will only have cache hits.

Your task is to use access times observed in this manner to determine the cache stride and therefore also the no. of ways.

## (Bonus) Task 3 - Find the cache line size

Devise a method to find the size of cache lines.
