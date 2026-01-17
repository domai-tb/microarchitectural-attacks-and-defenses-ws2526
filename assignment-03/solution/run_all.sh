#!/usr/bin/env bash
# run_all.sh
# - Builds all task binaries using make
# - Runs task1..task6 with example inputs and stores data under task*/data

set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
cd "$ROOT_DIR"

if command -v tput >/dev/null 2>&1 && tput sgr0 >/dev/null 2>&1; then
  BOLD="$(tput bold)"; DIM="$(tput dim)"; RESET="$(tput sgr0)"
  RED="$(tput setaf 1)"; GREEN="$(tput setaf 2)"; YELLOW="$(tput setaf 3)"; BLUE="$(tput setaf 4)"
else
  BOLD=""; DIM=""; RESET=""
  RED=""; GREEN=""; YELLOW=""; BLUE=""
fi

hr() { printf "%s\n" "------------------------------------------------------------"; }

section() {
  hr
  printf "%s%s%s\n" "$BOLD" "$1" "$RESET"
  hr
}

info()  { printf "%s[i]%s %s\n" "$BLUE" "$RESET" "$1"; }
ok()    { printf "%s[✓]%s %s\n" "$GREEN" "$RESET" "$1"; }
die()   { printf "%s[x]%s %s\n" "$RED" "$RESET" "$1"; exit 1; }

run_cmd() {
  local desc="$1"
  shift
  printf "%s\n" "${DIM}${desc}${RESET}"
  printf "%s$ %s%s\n" "$BOLD" "$*" "$RESET"
  "$@"
  printf "\n"
}

ensure_exec() {
  local p="$1"
  [[ -x "$p" ]] || die "Not executable (or missing): $p"
}

section "Build"
info "Running make to build all task binaries"
run_cmd "Build all targets" make
ok "Build completed."

section "Verify binaries"
for bin in task1/task1.bin task2/task2.bin task3/task3.bin task4/task4.bin task5/task5.bin task6/task6.bin; do
  ensure_exec "$bin"
  ok "Found executable: $bin"
done

mkdir -p task1/data task2/data task3/data task4/data task5/data task6/data

section "Task 1 – NOT gate"
run_cmd "Generating NOT accuracy data" true
./task1/task1.bin 2000 > task1/data/not.dat
ok "Saved task1/data/not.dat"

section "Task 2 – NAND/NOR gates"
run_cmd "Generating NAND/NOR accuracy data" true
./task2/task2.bin 1000 > task2/data/nand_nor.dat
ok "Saved task2/data/nand_nor.dat"

section "Task 3 – Multi-output gates"
run_cmd "Sweeping fan-out (samples=200, max_out=16)" true
./task3/task3.bin 200 16 > task3/data/fanout.dat
ok "Saved task3/data/fanout.dat"

section "Task 4 – AND/OR gates"
run_cmd "Generating AND/OR accuracy data" true
./task4/task4.bin 1000 > task4/data/and_or.dat
ok "Saved task4/data/and_or.dat"

section "Task 5 – XOR gate"
run_cmd "Generating XOR accuracy data" true
./task5/task5.bin 500 > task5/data/xor.dat
ok "Saved task5/data/xor.dat"

section "Task 6 – 7-segment LED"
run_cmd "Generating 7-seg accuracy data" true
./task6/task6.bin 200 > task6/data/seg7.dat
ok "Saved task6/data/seg7.dat"

section "Plots"
run_cmd "Generating summary plots" python3 plot_results.py
ok "Saved plots under: plots/"
