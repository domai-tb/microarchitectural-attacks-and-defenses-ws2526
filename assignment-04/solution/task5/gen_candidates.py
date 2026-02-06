#!/usr/bin/env python3
import argparse


def parse_hex(value: str) -> int:
    return int(value, 16)


def main() -> None:
    parser = argparse.ArgumentParser(
        description="Generate candidate addresses for init_uts_ns from a KASLR base range."
    )
    parser.add_argument("--base-start", required=True, type=parse_hex, help="Hex start base")
    parser.add_argument("--base-end", required=True, type=parse_hex, help="Hex end base (inclusive)")
    parser.add_argument("--step", required=True, type=parse_hex, help="Hex step size")
    parser.add_argument(
        "--symbol-offset", required=True, type=parse_hex, help="Hex offset of init_uts_ns"
    )
    parser.add_argument(
        "--output", default="task5/candidates.txt", help="Output file path"
    )
    args = parser.parse_args()

    if args.step <= 0:
        raise SystemExit("step must be > 0")
    if args.base_end < args.base_start:
        raise SystemExit("base-end must be >= base-start")

    with open(args.output, "w", encoding="utf-8") as f:
        base = args.base_start
        while base <= args.base_end:
            addr = base + args.symbol_offset
            f.write(f"0x{addr:x}\n")
            base += args.step


if __name__ == "__main__":
    main()
