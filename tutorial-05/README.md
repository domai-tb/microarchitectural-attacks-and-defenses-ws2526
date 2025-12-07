# Tutorial 5 - AssemblyLine

## Task 1 - AssemblyLine

Download AssemblyLine, build and install it. (When configuring, add the argument `--prefix=$HOME/usr` so the library is installed in your home directory.)

Use AssemblyLine to write a short function that takes a single integer (`uint64_t`) as input and returns the largest power of two that divides that integer, or 0 if the input is 0. Verify that your function works.

## Task 2 - mmap and AssemblyLine

Use `mmap` and AssemblyLine to generate the code of the function from Task 1 at an address which is a multiple of 1GB (230)

## Task 3 - Functions and caches

Write a program that repeats the following steps:

- use AssemblyLine to create a function that executes 10 NOPs and returns. The function should be created at a new address each time.
- Use `CLFLUSH` to flush the code of the function and the 20 cache lines that follow it
- Execute the function
- Measure which of the previously flushed cache lines is now in the cache.
- Delete everything it created
- Repeat these steps several times, to understand which cache lines are prefetched when executing an almost empty function

## Task 4 - Repeated execution

Repeat Task 3, but this time execute the function once before doing the `CLFLUSH`. What are the differences?

## Task 5 - Prefetching

Use AssemblyLine to create two functions. The first (short) consists of 10 NOPs and a return, and the second (long) consists of 512 NOPs and a return. Make sure that the addresses of the functions differ only in bit 32. (i.e., the two addresses are 4GB apart.) Execute the short function, and verify that in repeated executions, cache lines beyond the function are not prefetched. Then, verify that after executing the long function, executions of the short functions now prefetch cache lines beyond the function. How many lines are prefetched? Can you explain these results?

## Task 6 - more prefetching

Repeat Task 5, with different address bits. Find which address bits are used for determining prefetching.
