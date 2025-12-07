#!/usr/bin/env python3
"""
Heatmap analysis for task2.bin.

Usage (from solution/ directory):

    python3 task2/task2.py

This script:
  - Runs ./task2.bin for several predefined test cases.
  - Parses its stdout (lines: "sample set time").
  - Builds a 2D array [set, sample] of probe times.
  - Plots and stores heatmaps as PNGs (task2_<label>.png).

You can adjust TEST_CASES below to try different sets and sample counts.
"""

import subprocess
import sys
from pathlib import Path

import numpy as np
import matplotlib.pyplot as plt

# Adjust test cases as desired.
# Each case: number of samples, list of 10 rattled sets, label for output file name.
TEST_CASES = [
    {"samples": 200, "sets": [5, 7, 9, 11, 13, 15, 20, 22, 24, 26], "label": "case1"},
    {"samples": 200, "sets": [0, 1, 2, 3, 4, 5, 6, 7, 8, 9], "label": "case2"},
]

TASK2_TEST = "./task2.bin"
L1_SETS = 64


def run_task2_test(samples: int, sets: list[int]) -> np.ndarray:
    """
    Run ./task2.bin with the given arguments and return parsed data as
    an (N, 3) int64 array: columns [sample, set, time].
    """
    cmd = [TASK2_TEST, str(samples)] + [str(s) for s in sets]

    try:
        proc = subprocess.run(
            cmd,
            capture_output=True,
            text=True,
            check=True,
        )
    except FileNotFoundError:
        print(
            f"Error: {TASK2_TEST} not found. Build it first in the solution/ directory."
        )
        sys.exit(1)
    except subprocess.CalledProcessError as e:
        print(f"Error: {TASK2_TEST} failed with return code {e.returncode}")
        print("stderr:\n", e.stderr)
        sys.exit(1)

    stdout = proc.stdout.strip()
    if not stdout:
        print("Error: task2.bin produced no output.")
        sys.exit(1)

    # Convert text output "sample set time" into a NumPy array.
    # We expect three integers per line.
    try:
        # np.fromstring with sep=' ' will flatten all numbers,
        # then we reshape to (-1, 3).
        flat = np.fromstring(stdout, sep=" ", dtype=np.int64)
        if flat.size % 3 != 0:
            raise ValueError("unexpected number of columns in output")

        data = flat.reshape((-1, 3))
    except ValueError as e:
        print("Error while parsing task2.bin output:", e)
        sys.exit(1)

    return data


def build_heatmap(data: np.ndarray) -> np.ndarray:
    """
    Given data array with columns [sample, set, time],
    build a (L1_SETS, num_samples) heatmap of probe times.
    """
    samples = data[:, 0]
    sets = data[:, 1]
    times = data[:, 2]

    num_samples = int(samples.max()) + 1

    heat = np.zeros((L1_SETS, num_samples), dtype=np.float64)
    # Fill heat[set, sample] with time.
    for smp, s, t in zip(samples, sets, times):
        if 0 <= s < L1_SETS:
            heat[s, smp] = t

    return heat


def plot_heatmap(heat: np.ndarray, label: str, rattled_sets: list[int]) -> None:
    """
    Plot the heatmap and save it as <label>.png.
    Y-axis is cache set, X-axis is sample index.
    """
    num_samples = heat.shape[1]

    plt.figure(figsize=(10, 6))
    plt.imshow(
        heat,
        aspect="auto",
        origin="lower",
        interpolation="nearest",
        extent=[0, num_samples, 0, L1_SETS],
    )
    plt.colorbar(label="Probe time (cycles)")
    plt.xlabel("Sample number")
    plt.ylabel("Cache set (0–63)")
    plt.title(f"Task 2 heatmap – {label}\nRattled sets: {rattled_sets}")

    plt.tight_layout()
    out_name = f"./heatmaps/{label}.png"
    plt.savefig(out_name, dpi=200)
    plt.close()
    print(f"Saved heatmap to {out_name}")


def main() -> None:
    # Ensure we are in solution/ or adjust TASK2_TEST path accordingly.
    if not Path(TASK2_TEST).exists():
        print(f"Warning: {TASK2_TEST} not found in current directory.")
        print("Run this script from the solution/ directory where task2.bin is built.")
        sys.exit(1)

    for case in TEST_CASES:
        samples = case["samples"]
        sets = case["sets"]
        label = case["label"]

        if len(sets) != 10:
            print(f"Warning: test case {label} does not have exactly 10 sets.")

        print(
            f"Running task2.bin for case '{label}' with {samples} samples,"
            f" rattled sets {sets}..."
        )

        data = run_task2_test(samples, sets)

        # Optional: write raw data to a .dat file for inspection.
        dat_path = f"heatmaps/{label}.dat"
        np.savetxt(dat_path, data, fmt="%d")
        print(f"Saved raw data to {dat_path}")

        heat = build_heatmap(data)
        plot_heatmap(heat, label, sets)


if __name__ == "__main__":
    main()
