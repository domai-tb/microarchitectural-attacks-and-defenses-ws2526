# Tutorial 1 - Inline Assembly

Completion requirements
The aim of this tutorial is to get some practice with controlling the assembly code that the compiler produces. See https://gcc.gnu.org/onlinedocs/gcc/Extended-Asm.html for information on the syntax of inline assembly.

Please complete the tasks below.

Information on RUB VPN: https://noc.rub.de/web/vpn

## Task 1 - Sample Code with Inline Assembly

It's time to try some inline assembly. The code below implements a random number generator.

```
int getRandomNumber()
int result;
asm volatile ("mov %0, 4": "=r" (result));
return result;
```

The assembly instruction itself (mov %0, 4) is a template in which the compiler replaces %0 with the register it chooses for the first I/O operand. The operand is specified by "=r" (result), which maps the C variable result to the register.

Your task is to add a main that invokes the function and test the code output.

Please note that the asm instruction uses the non-default Intel format. To get the compiler to use this format, you should add the flag -masm=intel when compiling the code. Allternatively, you can use the equivalent code in AT&T format: movl $4, %0

For info on Inline Assembly:

https://gcc.gnu.org/onlinedocs/gcc-9.3.0/gcc/Extended-Asm.html

## Task 2 - Programming with Inline Assembly

Complete the inline assembly instruction in the function add below, so that the function returns the sum of its arguments.

```c
int add(int a, int b)
int result;
asm volatile ("<assembly instruction>": <output operands>: <input operands>);
return result;
```

## Task 3 - More Programming with Inline Assembly

Complete the inline assembly instruction in the function sum below, so that the function returns the sum of the first n elements of the array a. Note that the assembly template can include multiple instructions, separated by semicolons or new lines.

```c
#include <stdint.h>

int64_t sum(int64_t \*a, int64_t n)
int64_t result;
asm volatile ("<assembly instructions>": <output operands>: <input operands>: <clobbers>);
return result;
```

## Task 4 - Controlling Assembly Code

In this tasks, our aim is to control the assembly code that the compiler produces. Specifically, we start with the function

```c
void func()
    int c  = 0;

    for (int i = 0; i < 10; i++) {<br>      c++;</code><br><code>      asm volatile("");</code><br><code>    }
```

Our aim is that when it compiles (with optimization level -O3), the assembly code would look like:

```bash
func:
        mov     eax, 1
        mov     eax, 2
        mov     eax, 3
        mov     eax, 4
        mov     eax, 5
        mov     eax, 6
        mov     eax, 7
        mov     eax, 8
        mov     eax, 9
        mov     eax, 10
        ret
```

For that you can change the operand specifications of the asm instruction and add compilation pragmas.

There are several ways you can check the code. First you can compile with the -S argument, which produces the assembly in a corresponding .s file. For example, running gcc -masm=intel -S random.c will produce the file random.s that contains the assembly code. (Note that the code the compiler produces will include some more directives. We care only about the instructions.) Alternatively, you can visit https://godbolt.org, which provides a nice GUI interface and provides many choice. Make sure you choose C language with a recent version of GCC.
