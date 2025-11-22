import numpy as np
import matplotlib.pyplot as plt

# Load the trace from the output file
trace_data = np.loadtxt("trace.out")

# Extract p1 and p2 access times from the trace
p1_times = trace_data[:, 0]  # First column: p1 access times
p2_times = trace_data[:, 1]  # Second column: p2 access times

# Plot the collected trace
plt.figure(figsize=(12, 6))

# Plot p1 access times
plt.plot(p1_times, label="p1 Access Times", color="magenta")

# Plot p2 access times
plt.plot(p2_times, label="p2 Access Times", color="cyan")

plt.xlabel("Iterations")
plt.ylabel("Cycle Counts")
plt.title("Access Times for p1 and p2")
plt.legend()
plt.grid(True)

plt.savefig("task5.png", dpi=200)
plt.show()
