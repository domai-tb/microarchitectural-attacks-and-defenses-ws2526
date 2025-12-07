# Assignment 2 - Prime+Probe attack on AES

In this assignment you will implement a Prime+Probe attack on a T-table based implementation of AES. The submission consists of a PDF file with the required documentation, supporting code and data files, all packaged in a single tar archive. See detailed instruction within the tasks and under Submission below. Please pay attention to the instructions!

## Task 1 - L1-rattle with mmap (10%)

In this task, you will implement a rattle function that randomly accesses addresses mapped to user-specified cache sets.

First, allocate memory using mmap with flags MAP_PRIVATE | MAP_ANONYMOUS to allocate anonymous memory. Use lscpu --cache to determine your system's L1 cache parameters. Since L1 cache is virtually-indexed, you can calculate which addresses map to which cache sets using: cache_set = ((uintptr_t)addr >> 6) & 0x3F (assuming 64-byte cache lines and 64 sets).

Implement a rattle_cache_sets function that takes an array of target cache set numbers to rattle (1 ≤ n ≤ 64, where n is the number of sets). The rattle function should access addresses that are mapped to the specified cache sets. For each target set, select addresses from the allocated memory that map to that cache set, and repeatedly access them (e.g., 10,000 iterations). The random-access pattern should trigger cache activity and evictions within the target sets.

Your program should accept the following arguments (either via command-line or bash script): ./task1 <n> <set1> <set2> ... <setn> where set1 through setn are n cache set numbers (1 ≤ n ≤ 64) to rattle. The program should call the rattle function with the specified cache sets.

In your report, explain the working of your rattle function. No output is required for this task.

## Task 2 - Prime+Rattle+Probe on all sets (30%)

In this task, you will construct eviction sets for all 64 L1 cache sets, implement the Prime+Probe attack with rattle using the rattle_cache_sets function from Task 1, and create a heatmap visualisation of cache activity.

Allocate memory using mmap as described in Task 1. Iterate through the allocated memory, calculate which cache set each address maps to using the method from Task 1, and collect at least w addresses (w = associativity) for each cache set. You can use a circular linked lists for pointer chasing: each address should contain a pointer to the next address in the eviction set, with the last address pointing back to the first. This approach defeats CPU prefetchers.

Prime all 64 cache sets by accessing all addresses in your eviction sets using pointer chasing. Use the rattle_cache_sets function from Task 1 to rattle exactly 10 user-specified cache sets. Probe all 64 cache sets by measuring access times while traversing each eviction set element. The rattled sets should show higher access times (cache misses) compared to non-rattled sets (cache hits).

Your program should accept arguments: ./task2 <samples> <set1> <set2> ... <set10> where samples (e.g. 1000) is the number of samples, and set1 through set10 are exactly 10 cache set numbers (0-63) to rattle. Run the Prime+Rattle+Probe cycle for the specified number of samples. Export access time data to a file (e.g., task2.dat) in format: sample_number cache_set_number probe_time.

Write a script to generate a heatmap where the X-axis represents sample numbers, the Y-axis represents cache set numbers (0-63), and colour intensity represents probe time.

In your report, explain your Prime+Rattle+Probe implementations, and describe typical probe times for cache hits vs. misses. The output for this task is the cache activity heatmap.

## Task 3 - First round attack on AES (30%)

In this task, you will implement a Prime+Probe attack to recover the upper 4 bits of each AES key byte by monitoring cache accesses during the first round of AES encryption. You can use the eviction sets from Task 2. The AES implementation is provided here.

Recall that AES encryption uses T-tables, where the first round performs S-box[plaintext[i] ⊕ round_key[i]]. For each of the 16 key bytes, you need to collect multiple samples (typically ~ 100,000). For each sample, generate a random 16-byte plaintext block, probe all cache sets before encryption, execute AES encryption and then probe all cache sets to measure evictions. Cluster samples by the upper 4 bits of the plaintext byte (plaintext[byte] & 0xf0) and record probe times for each cache set. You might need to normalise the data by subtracting the overall average per cache set to remove baseline noise. To recover the key, for each key byte, try all 16 possible values for the upper 4 bits (0x0-0xF) and all 64 possible cache set offsets, then sum the probe times for the cache sets that would be accessed for each key guess. The correct key guess will show the maximum sum.

Your program should accept the following arguments: ./task3 [samples] [key] where samples is the number of samples per key byte, and key is a 32-character hex string representing the 16-byte AES key. Optionally, you can choose to read the key from a file. The output should show the upper nibbles (4 bits) of each guessed/recovered key byte in the stdout.

In your report, explain how the attack works and what measures you took to compensate for the system noise. Test with 3 different keys and report the accuracy (number of upper nibbles correctly recovered) for each tested key.

## Task 4 - Last round attack on AES (30%)

The first round attack can recover the upper nibbles of each byte. To recover the full key, one needs to perform a second-round attack, which requires complex analysis. However, the last round attack can recover the full 16 bytes due to the design of AES. In this task, you will implement a Prime+Probe attack to recover the last round key (K10) by monitoring cache accesses during the last round of AES encryption using ciphertext.

The last round of AES uses T-tables where the ciphertext is computed as ciphertext[i] = T-table[state[i]] ⊕ K10[i]. For each of the 16 key bytes, collect multiple samples (typically ~ 100,000). For each sample, generate a random 16-byte plaintext block, prime all cache sets before encryption, execute AES encryption and probe all cache sets to measure evictions. Cluster samples by the full ciphertext byte value (0-255) and record probe times for each cache set. Use normalization to remove baseline noise.

To recover K10 (last-round round-key), for each key byte try all 256 possible values (0x00-0xFF) and all 64 possible cache set offsets. For each key guess k and offset, compute the predicted cache set for each ciphertext value ct using:

```c
static const uint8_t AES_INV_SBOX[256] = {
        0x52, 0x09, 0x6a, 0xd5, 0x30, 0x36, 0xa5, 0x38, 0xbf, 0x40, 0xa3, 0x9e, 0x81, 0xf3, 0xd7, 0xfb,
        0x7c, 0xe3, 0x39, 0x82, 0x9b, 0x2f, 0xff, 0x87, 0x34, 0x8e, 0x43, 0x44, 0xc4, 0xde, 0xe9, 0xcb,
        0x54, 0x7b, 0x94, 0x32, 0xa6, 0xc2, 0x23, 0x3d, 0xee, 0x4c, 0x95, 0x0b, 0x42, 0xfa, 0xc3, 0x4e,
        0x08, 0x2e, 0xa1, 0x66, 0x28, 0xd9, 0x24, 0xb2, 0x76, 0x5b, 0xa2, 0x49, 0x6d, 0x8b, 0xd1, 0x25,
        0x72, 0xf8, 0xf6, 0x64, 0x86, 0x68, 0x98, 0x16, 0xd4, 0xa4, 0x5c, 0xcc, 0x5d, 0x65, 0xb6, 0x92,
        0x6c, 0x70, 0x48, 0x50, 0xfd, 0xed, 0xb9, 0xda, 0x5e, 0x15, 0x46, 0x57, 0xa7, 0x8d, 0x9d, 0x84,
        0x90, 0xd8, 0xab, 0x00, 0x8c, 0xbc, 0xd3, 0x0a, 0xf7, 0xe4, 0x58, 0x05, 0xb8, 0xb3, 0x45, 0x06,
        0xd0, 0x2c, 0x1e, 0x8f, 0xca, 0x3f, 0x0f, 0x02, 0xc1, 0xaf, 0xbd, 0x03, 0x01, 0x13, 0x8a, 0x6b,
        0x3a, 0x91, 0x11, 0x41, 0x4f, 0x67, 0xdc, 0xea, 0x97, 0xf2, 0xcf, 0xce, 0xf0, 0xb4, 0xe6, 0x73,
        0x96, 0xac, 0x74, 0x22, 0xe7, 0xad, 0x35, 0x85, 0xe2, 0xf9, 0x37, 0xe8, 0x1c, 0x75, 0xdf, 0x6e,
        0x47, 0xf1, 0x1a, 0x71, 0x1d, 0x29, 0xc5, 0x89, 0x6f, 0xb7, 0x62, 0x0e, 0xaa, 0x18, 0xbe, 0x1b,
        0xfc, 0x56, 0x3e, 0x4b, 0xc6, 0xd2, 0x79, 0x20, 0x9a, 0xdb, 0xc0, 0xfe, 0x78, 0xcd, 0x5a, 0xf4,
        0x1f, 0xdd, 0xa8, 0x33, 0x88, 0x07, 0xc7, 0x31, 0xb1, 0x12, 0x10, 0x59, 0x27, 0x80, 0xec, 0x5f,
        0x60, 0x51, 0x7f, 0xa9, 0x19, 0xb5, 0x4a, 0x0d, 0x2d, 0xe5, 0x7a, 0x9f, 0x93, 0xc9, 0x9c, 0xef,
        0xa0, 0xe0, 0x3b, 0x4d, 0xae, 0x2a, 0xf5, 0xb0, 0xc8, 0xeb, 0xbb, 0x3c, 0x83, 0x53, 0x99, 0x61,
        0x17, 0x2b, 0x04, 0x7e, 0xba, 0x77, 0xd6, 0x26, 0xe1, 0x69, 0x14, 0x63, 0x55, 0x21, 0x0c, 0x7d
};
for (int ct = 0; ct < 256; ct++) {
        // Compute S-box input: x = SB^{-1}(k XOR ct)
        uint8_t x = AES_INV_SBOX[ct ^ (uint8_t)k];
        int line = x >> 4; // Map to cache line (0-15)

        // Map to cache set: (offset + line) % L1_SETS (same pattern as first round attack)
        int set = (offset + line) % L1_SETS;

        // Sum normalized timing for this ciphertext value
        sum += data[ct][set];
}
```

Sum the probe times for the predicted cache sets. The correct key guess will show the maximum sum.

After recovering K10, you can verify the correctness by extracting the K10 using:

```c
AES_KEY aeskey;
private_AES_set_encrypt_key(key, 128, &aeskey);

// Extract K10 (round 10 key) from the key schedule
// For AES-128, round 10 key is at words 40-43 (indices 40-43)

uint8_t true_k10[16];
uint32_t *rk = aeskey.rd_key;
for (int i = 0; i < 4; i++) {
        uint32_t w = rk[40 + i];
        true_k10[i * 4 + 0] = (w >> 24) & 0xff;
        true_k10[i * 4 + 1] = (w >> 16) & 0xff;
        true_k10[i * 4 + 2] = (w >> 8) & 0xff;
        true_k10[i * 4 + 3] = w & 0xff;
}
```

Your program should accept arguments: ./task4 [samples] [key] where samples is the number of samples per key byte, and key is a 32-character hex string representing the 16-byte AES key. Optionally, you can choose to read the key from a file. The output should show the upper nibbles (4 bits) of each guessed/recovered key byte in the stdout.

In your report, explain how the attack works, how it differs from first first-round attack, and why does last-round attack reveals the whole key byte. Test with 3 different keys and report the accuracy (number of upper nibbles correctly recovered) for each tested key.

## Submission

As mentioned at the start, the submission is a single tar file (.tar or .tgz). The file format should match the extension. In particular, don't use a zip archive and rename it to .tar. The archive should contain source files, collected data files, a Makefile, and the file answers.pdf that contains your answers and the requested information. Do not include binaries, object files (.o), or anything that is generated from your sources.

Typing make in the folder extracted from your tar file should build all the programs requested above. Please try to follow a reasonable naming scheme and reasonably good code formatting. While we do not want to enforce a specific style and may overlook occasional style transgressions, we do want to read your code. The easier you make our lives, the easier it is for us to be generous
