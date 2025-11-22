#!/usr/bin/env bash

FILE="${1:-/home/victim/madgpg}"
OFFSET1="${2:-80000}"
OFFSET2="${3:-10000}"

# Make sure sudo can be used non-interactively (or enter password once)
sudo -v || { echo "sudo failed"; exit 1; }

# Start tracer in the background
./task6.bin "${FILE}" "${OFFSET1}" "${OFFSET2}" &
TRACE_PID=$!

# Tiny delay so tracer is ready before madgpg starts
sleep 0.05

# Trigger one RSA signature with the victim key
sudo -u victim /home/victim/madgpg --sign --output=/dev/null /dev/null

# Wait for tracer to finish
wait "${TRACE_PID}"

echo "Trace finished."
