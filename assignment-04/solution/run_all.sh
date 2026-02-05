#!/usr/bin/env bash
# run_all.sh
# - Builds all task binaries using make
# - Runs task1..task5 and stores data under task*/data

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
warn()  { printf "%s[!]%s %s\n" "$YELLOW" "$RESET" "$1"; }
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
for bin in task1/task1.bin task2/task2.bin task3/task3.bin task4/task4.bin task5/task5.bin; do
  ensure_exec "$bin"
  ok "Found executable: $bin"
done

mkdir -p task1/data task2/data task3/data task4/data task5/data

section "Task 1 – Covert channel (Flush+Reload)"
run_cmd "Generating channel accuracy data" true
./task1/task1.bin 50 > task1/data/cc_accuracy.dat
ok "Saved task1/data/cc_accuracy.dat"
info "Timing histogram CSV: task1/data/cc_timing.csv"

section "Task 2 – Basic meltdown (user-space sanity)"
run_cmd "Running user-space meltdown sanity test" true
./task2/task2.bin 200 > task2/data/meltdown_user.dat
ok "Saved task2/data/meltdown_user.dat"

section "Task 3 – Fault recovery"
run_cmd "Running recovery test" true
./task3/task3.bin 200 > task3/data/recovery.dat
ok "Saved task3/data/recovery.dat"

section "Task 4 – Leak init_uts_ns"
if [[ "$(id -u)" -ne 0 ]]; then
  warn "Skipping Task 4 (needs root to read /proc/kallsyms)."
else
  UTS_ADDR=$(awk '/ init_uts_ns$/ {print $1; exit}' /proc/kallsyms || true)
  if [[ -z "$UTS_ADDR" ]]; then
    warn "init_uts_ns not found in /proc/kallsyms."
  else
    echo "$UTS_ADDR" > task4/data/uts_addr.txt
    run_cmd "Leaking 256 bytes from init_uts_ns" true
    ./task4/task4.bin "$UTS_ADDR" 256 50 > task4/data/uts_dump.dat
    ok "Saved task4/data/uts_dump.dat (addr=$UTS_ADDR)"
  fi
fi

section "Task 5 – KASLR search"
if [[ -f task5/candidates.txt ]]; then
  run_cmd "Searching candidates from task5/candidates.txt" true
  ./task5/task5.bin task5/candidates.txt 50 > task5/data/kaslr_search.dat
  ok "Saved task5/data/kaslr_search.dat"
else
  warn "Skipping Task 5: provide task5/candidates.txt to run the search."
fi

section "Plots"
run_cmd "Generating plots" python3 plot_results.py
ok "Saved plots under: plots/"
