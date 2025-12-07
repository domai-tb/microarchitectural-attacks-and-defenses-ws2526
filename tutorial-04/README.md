# Tutorial 4 - Evict+Time

In this tutorial we will explore a variant of the Evict+Time attack. Evict+Time is based on the observation that when we evict a cache line from the cache, code that uses it will experience delays in execution. Thus, measuring code execution time can identify whether it uses the evicted cache line. The variant we will experiment with uses `clflush` instead of cache eviction. In most cases, this is a less realistic scenario, however it facilitates the attacks and will allow us to focus on the analysis of the data. Our victim is a T-table-based implementation of AES. For the attack, we will experiment with the implications of flushing a cache line of a T-table from the cache on the execution time of the function.

## Task 1 - Use the AES encryption code.

Download the implementation of AES, transfer to your lab machine, build and test. Make sure you know how to replace the key and the plaintext. You can verify the results using [public test vectors](https://github.com/ircmaxell/quality-checker/blob/master/tmp/gh_18/PHP-PasswordLib-master/test/Data/Vectors/aes-ecb.test-vectors) or an online [AES calculator](http://aes.online-domain-tools.com/).

## Task 2 - Measure AES execution time

Using the provided AES code, implement a program that generates 10000 random plaintexts and measures the time it takes to encrypt them with a fixed key. Perform basic statistical analysis to identify outliers and compute the mean and the standard deviation of the measured times (excluding outliers).

## Task 3 - Flush+Time attack

Find a way to flush the first cache line of table Te0 from the cache. You can "cheat" and modify the AES code - we are doing a proof-of-concept, not an attack. Repeat the measurements from Task 2. Is there a measurable statistical differences between the results?

## Task 4 - PoC for recovering a byte

Repeat the measurements of Task 3, this time with two groups of random plaintexts. The plaintexts in the first group are those for which first access in Te0 is to the first cache line, whereas the second group is for plaintexts where the first access to Te0 is to another cache line. Is there a statistical difference between the timings of the two groups?

## Task 5 - Recover the top 4 MSBs of the AES key bytes.

Design and implement an experiment that finds the four most significant bits of each key byte, under the assumption that you do not know what these are.

## Task 6 - Recover the full key

Can you extend the attack to round 2 to recover the whole key?
