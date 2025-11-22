#!/bin/bash
set -euo pipefail

if [ $# -ne 2 ]; then
    echo "Usage: $0 <file> <offset>"
    exit 1
fi

FILE=$1
OFFSET=$2
MONITOR_BIN="./task4" 
RESULT_DIR="results"

mkdir -p "$RESULT_DIR"

echo "Running experiments on file=$FILE offset=$OFFSET"
echo ""

for PERIOD in $(seq 1000 1000 20000); do
    OUTDIR="$RESULT_DIR/period_${PERIOD}"
    mkdir -p "$OUTDIR"

    echo "=== Period $PERIOD cycles ==="

    # 1) Run frtest and collect timestamps
    FRTEST_OUT="$OUTDIR/frtest.txt"

    echo "Running frtest..."
    /home/victim/frtest "$FILE" "$OFFSET" > "$FRTEST_OUT"
    echo "  frtest done → $FRTEST_OUT"

    # 2) Run your monitor program
    MONITOR_OUT="$OUTDIR/monitor.txt"

    echo "Running monitor..."
    "$MONITOR_BIN" "$FILE" "$OFFSET" "$PERIOD" > "$MONITOR_OUT"
    echo "  monitor done → $MONITOR_OUT"

    echo ""
done

echo "All experiments completed."
echo "Results stored under: $RESULT_DIR/"
