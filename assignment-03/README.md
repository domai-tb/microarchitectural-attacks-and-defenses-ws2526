# Assignment 3 - Weird Gates

In this assignment, you will implement the weird gates of Kaplan. Before you start, please read the paper, pay special attention to Sections IV, V, V-A, and V-B. In a nutshell, in those section Kaplan demonstrates how to treat cache state as a logical value, i.e. when a memory line is in the cache its logical value is TRUE, and when it is not in the cache, the logical value is FALSE. He then shows how to construct logical gates that operate on this state.

The assignment consists of 6 tasks. In each of the tasks you are asked to implement a logical gate. These gates/circuits are implemented as a function with input and output arguments. The functions operate on the logical state of the cache, but should never check the cache state, i.e. they should never measure whether a cache line is cached or not. In our testing, we will make sure that outputs are not in the cache when the function is called, so you can make that assumption. Your use may be different, and if needed you can flush outputs on entry to the function. For each of the tasks, you should provide the code, a short description of how the code works, and an evaluation of the accuracy of the gate or circuit.

## Task 1: Inverter (15%)

Implement the NOT gate. The function signature is `void NOT(void *out, void *in)`. You can use the code from Kaplan's paper, but you will have to tune it to the machines we use.

## Task 2: NOR/NAND gates (20%)

Implement the functions void `NOR(void *out, void *in1, void *in2)` and `void NAND(void *out, void *in1, void *in2)`

## Task 3: Multiple outputs (20%)

Implement the NOT, NAND, and NOR gates with multiple outputs. Report the maximum number of outputs that you can reliably support and the gate accuracy for each positional output.

## Task 4: AND/OR Gates (10%)

Implement AND and OR without using any of the gates you implemented in Tasks 1-3. These gates should be implemented with a single misprediction. Note that this task is likely more complex than others, because Kaplan does not show how to do it. At the same time, the task is not essential for any of the following tasks.

## Task 5: XOR Circuit (20%)

Combine the gates you have created to build a 2-input XOR gate.

## Task 6: 7-Segment LED (15%)

Design and implement a 7-segment LED circuit. The input is an array of four addresses each representing a bit in a hexadecimal digit. Use a little endian order, i.e. the least significant bit is in the first array entry. The output is an array of 7, representing the individual segments in the 7-segment LED, ordered top to bottom, then left to right. The specific function is:

| In  |     |     |     |     | Out |     |     |     |     |     |     |
| --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- |
| 0   | 1   | 2   | 3   |     | 0   | 1   | 2   | 3   | 4   | 5   | 6   |
| --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- |
| 0   | 0   | 0   | 0   |     | 1   | 1   | 1   | 0   | 1   | 1   | 1   |
| 1   | 0   | 0   | 0   |     | 0   | 0   | 1   | 0   | 0   | 1   | 0   |
| 0   | 1   | 0   | 0   |     | 1   | 0   | 1   | 1   | 1   | 0   | 1   |
| 1   | 1   | 0   | 0   |     | 1   | 0   | 1   | 1   | 0   | 1   | 1   |
| 0   | 0   | 1   | 0   |     | 0   | 1   | 1   | 1   | 0   | 1   | 0   |
| 1   | 0   | 1   | 0   |     | 1   | 1   | 0   | 1   | 0   | 1   | 1   |
| 0   | 1   | 1   | 0   |     | 1   | 1   | 0   | 1   | 1   | 1   | 1   |
| 1   | 1   | 1   | 0   |     | 1   | 0   | 1   | 0   | 0   | 1   | 0   |
| 0   | 0   | 0   | 1   |     | 1   | 1   | 1   | 1   | 1   | 1   | 1   |
| 1   | 0   | 0   | 1   |     | 1   | 1   | 1   | 1   | 0   | 1   | 0   |
| 0   | 1   | 0   | 1   |     | 1   | 1   | 1   | 1   | 1   | 1   | 0   |
| 1   | 1   | 0   | 1   |     | 0   | 1   | 0   | 1   | 1   | 1   | 1   |
| 0   | 0   | 1   | 1   |     | 1   | 1   | 0   | 0   | 1   | 0   | 1   |
| 1   | 0   | 1   | 1   |     | 0   | 0   | 1   | 1   | 1   | 1   | 1   |
| 0   | 1   | 1   | 1   |     | 1   | 1   | 0   | 1   | 1   | 0   | 1   |
| 1   | 1   | 1   | 1   |     | 1   | 1   | 0   | 1   | 1   | 0   | 0   |

## Submission

The submission is a single tar file (.tar or .tgz). The archive should contain source files, collected data files, a Makefile, and the file answers.pdf that explains your approach, observations and result for each task. Also, mention where can we find the relevant source code for each task in the report. Do not include binaries, object files (.o), or anything that is generated from your sources.
