# Tutorial 6 - Prefetching

In this tutorial, we continue the investigation of the BTB and the instruction prefetcher. Last week, you should have found out that the BTB ignores bits 30 and above when determining the source of the branch.

Note: AssemblyLine is optional—inline assembly or an external assembly function works too. AssemblyLine simply makes it easier.

## Task 1 - Test prefetch with branches

Use assemblyline to write two functions. The first (trainer) has 10 NOPs, followed by a branch forward by 300 bytes, where the return instruction is. The second (trainee) has 10 NOPs and a return. The two functions should be at aliased addresses, i.e. addresses that only differ in bit 32.

Use a procedure similar to last week: Execute the trainer, then flush the trainee and the 20 follwing cache lines. Execute the trainee and verify that it has been trained.

## Task 2 - Aliasing function

The [BunnyHop paper](https://www.usenix.org/system/files/usenixsecurity23-zhang-zhiyuan-bunnyhop.pdf) argues that the function is a bit more complex. Specifically, they state that the BTB is a set-associative cache, with set number determined by bits [13:5] of the branch address (last byte of the branching instruction), and the tag is determine by a XOR of bits [29:22] and bits [21:14]. For aliasing to occur, i.e. for one branch to train another, both the tag and the set should match.

Repeat Task 1, only now, use an aliased address that differs in bits below 30.
