# Assignment 4 - Meltdown

In this assignment we will implement a Meltdown attack against the operating system kernel on the lab machines.
The file [meltdown-template.c](./material/meltdown-template.c) includes some basic code that you will complete.

## Task 1 - Build a covert channel (30%)

The covert channel consists of four functions. The function `cc_init()` initializes the covert channel, setting up whatever memory it needs. `cc_setup()` sets the channel for sending a byte. `cc_transmit(uint8_t value)` transmits a value through the channel, and `int cc_receive()` returns the value it receives, or -1 if no value has been received. In the case that multiple values appear to have been sent, the function can arbitrarily choose which one to return.

For a channel, we will use the Flush+Reload attack. Basically, `cc_init` allocates space for 256 different cache lines that the transmit function uses. `cc_setup` flushes these lines from the cache, `cc_transmit` accesses one of these cache lines based on the transmitted value, and `cc_receive` checks which of the cache lines is cached. To overcome prefetchers, we want each of these cache lines to reside in a different memory page.

Your task is to implement the four functions, test the accuracy of the covert channel, and report.

## Task 2 - Explaining the basic Meltdown code (10%)

The provided function `void meltdown(uintptr_t adrs)` performs the core fo the attack. It takes an address (`uintptr_t`) as an argument, reads it and transmits the contents through a covert channel it has set up. In addition, it does some other operations. Your task is to explain why these are needed. You may want to delay answering this until after you have the attack working, when experimenting will help.

## Task 3 - Recovering from faults (30%)

The main problem of the code in `meltdown` is that if called with an invalid address, it crashes. In this task, you will implement the function `int do_meltdown(uintptr_t adrs)`, which calls meltdown, recovers from the fault, receives the transmitted value from the channel, and returns it. You may want to read the man pages of `signal(2)` and `sigsetjmp(3)`. Make sure that you can call `do_meltdown` several times without crash or hang. You may assume that the program does some one-time setup before calling do_meltdown.
In your submission, explain how you implement the function.

## Task 4 - Performing the attack (25%)

We are now ready to perform the attack. We will try to use it to read the contents of the structure `init_uts_ns`. This is a structure of type `struct new_utsname`, defined in `/usr/include/linux/utsname.h`, that contains some information about the system. Now, there are a couple of problems that you need to tackle:

- Due to kernel address layout randomization (KASLR), the address of `init_uts_ns` is not fixed, and changes every time the system reboots. The file `/proc/kallsyms` lists the addresses of all of the kernel symbols. To get the contents of the file, you can use `sudo /usr/bin/cat /proc/kallsyms`. Once you find the address of the structure, you can use your implementation of Meltdown to try and read it.
- Technically, Meltdown only reads data in the L1 cache. There is no guarantee that `init_uts_ns` is cached. Fortunately, due to the use of signal handlers for recovery, the system sometimes caches the memory you did not access, so repeating the attack will eventually bring the data in (See [Schwarzl et al.](./material/spec_deref_extended.pdf) for more information).
- We find that in many cases Meltdown returns the value 0, even though the contents of the memory is different. To fix, repeat the attack several times and store a non-zero value if received. We find that when trying to read multiple addresses, it's better to cycle through the addresses multiple times instead of reading each address multiple times before moving to the next time. This may be implementation dependent and your results may vary.
- You may want to have some ground truth for correct values read. We know that offset 65 from the start of `init_uts_ns` contains the character 'p' and offset 66 contains 'c'.

Your task is to recover the contents of the structure.

## Task 5 - Overcoming KASLR (5%)

Your task here is to use the Meltdown attack together with the (now) known contents of `init_uts_ns` to overcome KASLR. The core idea is to first understand how KASLR can randomize addresses. This allows generating a list of all possible addresses for `init_uts_ns`. Then, by running the Meltdown attack, you can compare the contents of the memory to the expected contents of the structure and infer the address. You can use `sudo /sbin/reboot` to reboot the system, which will change the address of `init_uts_ns`.

Please describe how your code works and provide some information on the time it takes to perform the attack and on the success rate.

**This task is quite challenging and the marks do not reflect the expected effort.**
