import numpy as np
import matplotlib.pyplot as plt
import os
import re

RESULT_ROOT = "results"


def load_timestamps(path):
    """Load timestamps, return empty array if file missing or not readable."""
    try:
        # Using genfromtxt to handle potential missing or malformed lines
        timestamps = np.genfromtxt(path, dtype=np.float64)
        # Remove NaNs and any invalid values
        timestamps = timestamps[~np.isnan(timestamps)]
        return timestamps
    except Exception as e:
        print(f"Error reading {path}: {e}")
        return np.array([])  # Return empty array on failure


def compute_overlap(frtest_ts, monitor_ts, tolerance=1000):
    """
    Compute overlap probability.
    Returns fraction of frtest events that fall within tolerance
    of any monitor detection.
    """
    if len(frtest_ts) == 0 or len(monitor_ts) == 0:
        return 0.0  # If either list is empty, no overlap can be calculated

    overlaps = 0
    for ts in frtest_ts:
        # Check if monitor detected near this timestamp
        if np.any(np.abs(monitor_ts - ts) <= tolerance):
            overlaps += 1

    return overlaps / len(frtest_ts)


def main():
    periods = []  # List to hold slot lengths (periods)
    overlap_values = []  # List to hold the corresponding overlap probabilities

    # Scan all period_* directories
    for dirname in sorted(os.listdir(RESULT_ROOT)):
        m = re.match(r"period_(\d+)", dirname)
        if not m:
            continue

        period = int(m.group(1))  # Slot length in cycles (x-axis)
        fr_path = os.path.join(RESULT_ROOT, dirname, "frtest.txt")
        mon_path = os.path.join(RESULT_ROOT, dirname, "monitor.txt")

        # Load timestamps from both files
        fr_ts = load_timestamps(fr_path)
        mon_ts = load_timestamps(mon_path)

        # Compute overlap and append results
        overlap = compute_overlap(fr_ts, mon_ts)
        periods.append(period)  # Add period to x-axis list
        overlap_values.append(overlap)  # Add overlap to y-axis list

    # Convert periods and overlap_values to NumPy arrays for ease of manipulation
    periods = np.array(periods)
    overlap_values = np.array(overlap_values)

    # ---- Plot ----
    plt.figure(figsize=(10, 5))
    plt.plot(periods, overlap_values, "bx", label="Measured")  # Plot period vs overlap

    # Add labels and title
    plt.xlabel("Slot Length (cycles)")  # x-axis: Slot length in cycles
    plt.ylabel("Overlap Probability")  # y-axis: Overlap probability
    plt.title("Monitoring Accuracy")  # Title for the plot
    plt.grid(True)  # Show gridlines for better readability
    plt.legend()  # Display legend
    plt.tight_layout()  # Adjust layout to prevent clipping

    # Save the plot as a PNG file
    plt.savefig("task4.png", dpi=200)  # Save the figure with high resolution
    print("Saved plot → task4.png")  # Print confirmation


if __name__ == "__main__":
    main()
