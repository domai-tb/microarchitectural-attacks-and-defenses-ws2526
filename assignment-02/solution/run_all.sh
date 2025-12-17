#!/usr/bin/env bash
# run_all.sh
# - Builds all task binaries using make
# - Runs task1..task4 with example inputs

set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
cd "$ROOT_DIR"

# ---------------------------- helpers ----------------------------

if command -v tput >/dev/null 2>&1; then
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

ensure_exists() {
  local p="$1"
  [[ -f "$p" ]] || die "Missing file: $p"
}

ensure_exec() {
  local p="$1"
  [[ -x "$p" ]] || die "Not executable (or missing): $p"
}

# ---------------------------- build ----------------------------

section "Build"
info "Running make to build all task binaries"
run_cmd "Build all targets" make
ok "Build completed."

# ---------------------------- verify artifacts ----------------------------

section "Verify binaries"
for bin in task1/task1.bin task2/task2.bin task3/task3.bin task4/task4.bin; do
  ensure_exec "$bin"
  ok "Found executable: $bin"
done

# ---------------------------- Task 1 ----------------------------

section "Task 1 – L1 rattle with mmap"
info "Goal: Access addresses mapped to user-chosen L1 cache sets (no output required by assignment)."
info "Smoke test: rattle 3 sets."

TASK1_SETS=(0 10 42)
TASK1_N="${#TASK1_SETS[@]}"

run_cmd \
  "Running Task 1: rattling sets ${TASK1_SETS[*]}" \
  ./task1/task1.bin "$TASK1_N" "${TASK1_SETS[@]}"

ok "Task 1 executed successfully."

# ---------------------------- Task 2 ----------------------------

section "Task 2 – Prime+Rattle+Probe + heatmaps"
info "Goal: Prime all sets, rattle 10 chosen sets, probe all sets, and visualize cache activity as heatmaps."
info "This uses task2/task2.py which runs multiple test cases itself and stores .dat/.png in task2/heatmaps/."

ensure_exists "task2/task2.py"
mkdir -p "task2/heatmaps"

run_cmd \
  "Running Task 2: generating raw traces and heatmaps (task2/heatmaps/*)" \
  python3 task2/task2.py

ok "Task 2 executed successfully."
info "Check outputs under: task2/heatmaps/"
info "Expected: case*.dat and case*.png"

# ---------------------------- Task 3 ----------------------------

section "Task 3 – First round AES attack (upper nibbles)"
info "Goal: Recover upper 4 bits of each AES key byte via first-round Prime+Probe."
info "Demo run with a known key."

TASK3_SAMPLES=20000
TASK3_KEY="00112233445566778899aabbccddeeff"

run_cmd \
  "Running Task 3: samples=${TASK3_SAMPLES}, key=${TASK3_KEY} (prints 16 hex digits)" \
  ./task3/task3.bin "$TASK3_SAMPLES" "$TASK3_KEY"

ok "Task 3 executed successfully."

# ---------------------------- Task 4 ----------------------------

section "Task 4 – Last round AES attack (recover K10)"
info "Goal: Recover full last-round key K10 bytes via ciphertext clustering and INV_SBOX model."
info "Demo run with the same key."

TASK4_SAMPLES=20000
TASK4_KEY="$TASK3_KEY"

run_cmd \
  "Running Task 4: samples=${TASK4_SAMPLES}, key=${TASK4_KEY} (prints recovered K10 / accuracy)" \
  ./task4/task4.bin "$TASK4_SAMPLES" "$TASK4_KEY"

ok "Task 4 executed successfully."

# ---------------------------- summary ----------------------------

section "Summary"
ok "All tasks executed successfully."
