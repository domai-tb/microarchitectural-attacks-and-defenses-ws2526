#!/usr/bin/env python3
import csv
from collections import defaultdict
from pathlib import Path

import matplotlib.pyplot as plt

ROOT = Path(__file__).resolve().parent
PLOTS_DIR = ROOT / "plots"
PLOTS_DIR.mkdir(exist_ok=True)


def load_simple_accuracy(path, gate_filter=None):
    total = 0
    correct = 0
    with path.open() as f:
        for line in f:
            line = line.strip()
            if not line:
                continue
            parts = line.split()
            if gate_filter is not None:
                if parts[0] != gate_filter:
                    continue
                expected = int(parts[3])
                observed = int(parts[4])
            else:
                expected = int(parts[1])
                observed = int(parts[2])
            total += 1
            correct += (expected == observed)
    return 0.0 if total == 0 else 100.0 * correct / total


def load_gate_accuracy(path):
    acc = {}
    with path.open() as f:
        for line in f:
            line = line.strip()
            if not line:
                continue
            gate, a, b, expected, observed = line.split()
            expected = int(expected)
            observed = int(observed)
            acc.setdefault(gate, [0, 0])
            acc[gate][0] += (expected == observed)
            acc[gate][1] += 1
    return {g: 100.0 * c / t for g, (c, t) in acc.items()}


def load_xor_accuracy(path):
    return load_gate_accuracy(path).get("XOR", 0.0)


def load_seg_accuracy(path):
    total = 0
    correct = 0
    with path.open() as f:
        for line in f:
            line = line.strip()
            if not line:
                continue
            _, _, expected, observed = line.split()
            expected = int(expected)
            observed = int(observed)
            total += 1
            correct += (expected == observed)
    return 0.0 if total == 0 else 100.0 * correct / total


def load_fanout(path):
    per_gate = defaultdict(lambda: defaultdict(list))
    with path.open() as f:
        for line in f:
            line = line.strip()
            if not line:
                continue
            gate, n_out, out_idx, acc = line.split()
            per_gate[gate][int(n_out)].append(float(acc))
    return per_gate


def plot_summary(summary):
    labels = list(summary.keys())
    values = [summary[k] for k in labels]
    fig, ax = plt.subplots(figsize=(8, 4))
    ax.bar(labels, values, color="#2f4f4f")
    ax.set_ylabel("Accuracy (%)")
    ax.set_ylim(0, 100)
    ax.set_title("Gate Accuracy Summary")
    ax.grid(axis="y", alpha=0.3)
    fig.tight_layout()
    fig.savefig(PLOTS_DIR / "summary.png", dpi=150)
    plt.close(fig)


def plot_fanout(per_gate):
    fig, ax = plt.subplots(figsize=(8, 4))
    for gate, series in sorted(per_gate.items()):
        xs = sorted(series.keys())
        ys = [sum(series[n]) / len(series[n]) for n in xs]
        ax.plot(xs, ys, marker="o", label=gate)
    ax.set_xlabel("Outputs")
    ax.set_ylabel("Avg Accuracy (%)")
    ax.set_title("Multi-output Accuracy (Average per n_out)")
    ax.set_ylim(0, 100)
    ax.grid(alpha=0.3)
    ax.legend()
    fig.tight_layout()
    fig.savefig(PLOTS_DIR / "fanout.png", dpi=150)
    plt.close(fig)


def main():
    summary = {}
    summary["NOT"] = load_simple_accuracy(ROOT / "task1/data/not.dat")

    task2_acc = load_gate_accuracy(ROOT / "task2/data/nand_nor.dat")
    summary.update(task2_acc)

    task4_acc = load_gate_accuracy(ROOT / "task4/data/and_or.dat")
    summary.update(task4_acc)

    summary["XOR"] = load_xor_accuracy(ROOT / "task5/data/xor.dat")
    summary["7SEG"] = load_seg_accuracy(ROOT / "task6/data/seg7.dat")

    with (PLOTS_DIR / "summary.csv").open("w", newline="") as f:
        writer = csv.writer(f)
        writer.writerow(["gate", "accuracy_percent"])
        for gate, acc in summary.items():
            writer.writerow([gate, f"{acc:.2f}"])

    plot_summary(summary)

    per_gate = load_fanout(ROOT / "task3/data/fanout.dat")
    plot_fanout(per_gate)


if __name__ == "__main__":
    main()
