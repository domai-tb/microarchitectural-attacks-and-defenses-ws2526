#!/usr/bin/env python3
import csv
from pathlib import Path

import matplotlib.pyplot as plt
import numpy as np

ROOT = Path(__file__).resolve().parent
PLOTS_DIR = ROOT / "plots"
PLOTS_DIR.mkdir(exist_ok=True)


def load_timing(path):
    cached = []
    uncached = []
    if not path.exists():
        return cached, uncached
    with path.open() as f:
        reader = csv.reader(f)
        next(reader, None)
        for row in reader:
            if len(row) < 2:
                continue
            typ, cycles = row[0], int(row[1])
            if typ == "cached":
                cached.append(cycles)
            else:
                uncached.append(cycles)
    return cached, uncached


def load_accuracy(path):
    total = 0
    correct = 0
    if not path.exists():
        return 0.0
    with path.open() as f:
        for line in f:
            line = line.strip()
            if not line:
                continue
            parts = line.split()
            if len(parts) < 2:
                continue
            expected = int(parts[0])
            observed = int(parts[1])
            total += 1
            correct += (expected == observed)
    return 0.0 if total == 0 else 100.0 * correct / total


def plot_timing(cached, uncached):
    if not cached and not uncached:
        return
    c_max = max(cached) if cached else 0
    u_max = max(uncached) if uncached else 0
    max_v = max(c_max, u_max, 1000)
    bins = np.linspace(0, max_v, 200)

    plt.figure(figsize=(10, 5))
    plt.hist(cached, bins=bins, alpha=0.6, label="cached")
    plt.hist(uncached, bins=bins, alpha=0.6, label="uncached")
    plt.xlabel("Cycles")
    plt.ylabel("Count")
    plt.title("Flush+Reload Timing Distribution")
    plt.legend()
    plt.tight_layout()
    plt.savefig(PLOTS_DIR / "cc_timing.png", dpi=150)
    plt.close()


def plot_accuracy(acc):
    fig, ax = plt.subplots(figsize=(4, 4))
    ax.bar(["channel"], [acc], color="#2f4f4f")
    ax.set_ylabel("Accuracy (%)")
    ax.set_ylim(0, 100)
    ax.set_title("Covert Channel Accuracy")
    ax.grid(axis="y", alpha=0.3)
    fig.tight_layout()
    fig.savefig(PLOTS_DIR / "cc_accuracy.png", dpi=150)
    plt.close(fig)


def main():
    cached, uncached = load_timing(ROOT / "task1/data/cc_timing.csv")
    plot_timing(cached, uncached)

    acc = load_accuracy(ROOT / "task1/data/cc_accuracy.dat")
    with (PLOTS_DIR / "cc_accuracy.csv").open("w", newline="") as f:
        writer = csv.writer(f)
        writer.writerow(["metric", "value"])
        writer.writerow(["accuracy_percent", f"{acc:.2f}"])

    plot_accuracy(acc)


if __name__ == "__main__":
    main()
