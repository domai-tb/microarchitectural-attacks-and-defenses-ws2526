# Tutorial 2 - Timing and memory mapping

In this tutorial we will work on the basics of measuring operation time and on mapping data within the address space.

## Task 1 - measure cache access time

In the first task, we will measure how long a memory access takes. We will use the rdtscp instruction for the measurement.  The main challenge is to ensure that we measure the instruction we want, with a fixed overhead. The details are in this Intel manual. The short story is that rdtscp ensures that it executes only after all preceding (older) instructions completed, but does not guarantee that younger instructions don't execute before it. Hence, we need to combine it with lfence, which ensures a strict fence.  Another challenge is that the processor adapts its frequency to the load, which changes the timing.  Looping for 109 cycles seems to bring the processor to a reasonably high frequency that allows mostly-stable measurements. Finally, you need to make sure that the caching status of the memory you access is fixed. The easiest solution is to make sure it is cached.

Your task is: measure the time for a single memory access to a cached line.  Verify that you are actually measuring the access time by checking the assembly code of your program.

Note that your measurements include some overhead due to the rdtscp and lfence instructions. How can you find out the actual cache access time?

## Task 2 - measure memory access time

To meaasure the memory access time, you need to ensure that the line you access is not in  the cache. The clflush instruction removes a memory line from the cache. The problem here is that you need to wait for the operation to complete. The two ways of doing that are (1) using the mfence instruction, which guarntees to be ordered with respect to clflush, and (2) to wait for 1000 cycles. In practice, we get best results by doing both.

Your task is: measure the time for a single memory access to a memory line that is not cached. 

 Example of `clflush`: asm volatile("clflush 0(%0)\n" : : "c"(&var) : "memory");

## Task 3 - mapping a file

Create a text file, longer than 4096 bytes, and use the mmap system call to map its first page (for read-only) into a program's address space. Verify that you got the file's contents in memory.  

## Task 4 - multiple mappings

Create another mapping of the same file as in Task 3 into memory. You now have two addresses that point to the same contents. Devise a test to check whether the two copies are shared without writing to the memory. 

## Task 5 - anonymous memory

Use the mmap system call to create an anonymous page (i.e. one that is not attached to a file).  Verify that the contents is all 0.

## Task 6 - more anonymous memory

Map two anonymous pages into memory. Without writing to them, verify whether they are shared. Verify whether using read/write mapping changes the sharing status. What about writing zero to either or to both pages?