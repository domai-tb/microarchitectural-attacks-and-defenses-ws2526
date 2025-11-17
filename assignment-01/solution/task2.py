#!/usr/bin/env python3

import csv
import numpy as np
import matplotlib.pyplot as plt

cached = []
uncached = []

with open("task2.csv", newline="") as f:
    reader = csv.reader(f)
    next(reader)  # header
    for row in reader:
        typ, cycles = row[0], int(row[1])
        if typ == "cached":
            cached.append(cycles)
        else:
            uncached.append(cycles)

cached = np.array(cached)
uncached = np.array(uncached)


def stats(arr):
    return arr.mean(), np.median(arr), arr.std(), arr.min(), arr.max()


c_mean, c_median, c_std, c_min, c_max = stats(cached)
u_mean, u_median, u_std, u_min, u_max = stats(uncached)

print(
    "Cached:   mean={:.2f}, median={:.2f}, std={:.2f}, min={}, max={}".format(
        c_mean, c_median, c_std, c_min, c_max
    )
)
print(
    "Uncached: mean={:.2f}, median={:.2f}, std={:.2f}, min={}, max={}".format(
        u_mean, u_median, u_std, u_min, u_max
    )
)

# histogram
plt.figure(figsize=(10, 5))
bins = np.linspace(0, max(c_max, u_max, 1000), 200)
plt.hist(cached, bins=bins, alpha=0.6, label="cached")
plt.hist(uncached, bins=bins, alpha=0.6, label="uncached")
plt.xlabel("Cycles")
plt.ylabel("Count")
plt.title("Flush+Reload access time distribution")
plt.legend()
plt.tight_layout()
plt.savefig("flush_reload_histogram.png")
print("Saved histogram to flush_reload_histogram.png")
