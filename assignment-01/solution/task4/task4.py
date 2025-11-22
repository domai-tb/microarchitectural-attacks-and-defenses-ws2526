#!/usr/bin/env python3
import numpy as np
import matplotlib.pyplot as plt
import os
import re

RESULT_ROOT = "results"


def load_timestamps(path):
    """Load timestamps, return empty array if file missing."""
    try:
        return np.loadtxt(path)
    except:
        return np.array([])


def compute_overlap(frtest_ts, monitor_ts, tolerance=2000):
    """
    Compute overlap probability.
    Returns fraction of frtest events that fall within tolerance
    of any monitor detection.
    """
    if len(frtest_ts) == 0:
        return 0.0
    if len(monitor_ts) == 0:
        return 0.0

    overlaps = 0
    for ts in frtest_ts:
        # Check if monitor detected near this timestamp
        if np.any(np.abs(monitor_ts - ts) <= tolerance):
            overlaps += 1

    return overlaps / len(frtest_ts)


def main():
    periods = []
    overlap_values = []

    # Scan all period_* directories
    for dirname in sorted(os.listdir(RESULT_ROOT)):
        m = re.match(r"period_(\d+)", dirname)
        if not m:
            continue

        period = int(m.group(1))
        fr_path = os.path.join(RESULT_ROOT, dirname, "frtest.txt")
        mon_path = os.path.join(RESULT_ROOT, dirname, "monitor.txt")

        fr_ts = load_timestamps(fr_path)
        mon_ts = load_timestamps(mon_path)

        overlap = compute_overlap(fr_ts, mon_ts)
        periods.append(period)
        overlap_values.append(overlap)

    periods = np.array(periods)
    overlap_values = np.array(overlap_values)

    # ---- Fit a decay model (optional, red curve) ----
    # simple fit: a / (x + b)
    if len(periods) > 0:

        def model(x, a, b):
            return a / (x + b)

        try:
            from scipy.optimize import curve_fit

            popt, _ = curve_fit(
                model, periods, overlap_values, p0=[1.0, 1.0], maxfev=5000
            )
            xfit = np.linspace(periods.min(), periods.max(), 500)
            yfit = model(xfit, *popt)
            fit_available = True
        except:
            fit_available = False

    # ---- Plot ----
    plt.figure(figsize=(10, 5))
    plt.plot(periods, overlap_values, "bx", label="Measured")

    if len(periods) > 0 and "fit_available" in locals() and fit_available:
        plt.plot(xfit, yfit, "r-", label="Fitted Model")

    plt.xlabel("Slot Length (cycles)")
    plt.ylabel("Overlap Probability")
    plt.title("Monitoring Accuracy")
    plt.grid(alpha=0.25)
    plt.legend()
    plt.tight_layout()

    plt.savefig("task4.png", dpi=200)
    print("Saved plot → task4.png")


if __name__ == "__main__":
    main()
