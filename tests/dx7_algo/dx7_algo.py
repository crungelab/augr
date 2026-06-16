#!/usr/bin/env python3
"""
dx7_algo.py — two tools in one, for fixing the kAlgorithms table.

  1. dump   : read a .syx cartridge and report, per patch, the algorithm
              number, feedback amount, and per-operator output levels
              plus full EG rates (R1-R4) and levels (L1-L4).
              This is GROUND TRUTH from your file — it tells you which
              algorithms you actually need to get right, so you can
              prioritise verifying those against Dexed first.

  2. gen    : compile a human-readable algorithm spec (the way you'd read
              it straight off Dexed's diagram) into the exact C++
              kAlgorithms[] entries, handling every index conversion,
              carrier-flag derivation, route count, and feedback_op
              placement automatically, with validation.

The routing data is NOT in the .syx — it's fixed DX7 hardware. So `gen`
works from the SPECS table below, which you fill in by reading Dexed's
algorithm diagrams. One verified example (alg 22) is provided to show
the format.

Usage:
    python3 dx7_algo.py dump rom1a.syx
    python3 dx7_algo.py gen
"""

import sys
import struct

# ----------------------------------------------------------------------
# Packed-voice byte offsets (verified against dxsyx.cpp / SYX_VOICE_SIZE=128)
# ----------------------------------------------------------------------
HEADER       = 6
VOICE_SIZE   = 128
OP_SIZE      = 17       # packed operator stride
OP_OUTLEVEL  = 14       # output level, operator-relative
OP_RATES     = (0, 1, 2, 3)   # EG R1..R4, operator-relative
OP_LEVELS    = (4, 5, 6, 7)   # EG L1..L4, operator-relative
OFF_ALGO     = 110      # voice-relative
OFF_FEEDBACK = 111
OFF_NAME     = 118
NAME_LEN     = 10


def _fix_char(b):
    return chr(b) if 32 <= b < 127 else '_'


def dump_cartridge(path):
    with open(path, 'rb') as f:
        data = f.read()

    print(f"file: {path}  ({len(data)} bytes)")
    if len(data) < HEADER + VOICE_SIZE * 32 + 2:
        print("  WARNING: smaller than a 32-voice cartridge")
    if data[0] != 0xF0 or data[1] != 0x43 or data[3] != 0x09:
        print("  WARNING: header does not look like a DX7 32-voice dump")

    algo_usage = {}
    print()
    for v in range(32):
        base = HEADER + v * VOICE_SIZE
        voice = data[base:base + VOICE_SIZE]
        if len(voice) < VOICE_SIZE:
            break

        algo = (voice[OFF_ALGO] & 0x1F) + 1        # 1-based for display
        fb   = voice[OFF_FEEDBACK] & 0x07
        name = ''.join(_fix_char(b) for b in voice[OFF_NAME:OFF_NAME + NAME_LEN]).rstrip()

        # Operators are stored OP6..OP1; reverse so out_levels[0] == OP1.
        out_levels = [0] * 6
        eg_rates   = [None] * 6   # each entry: (R1,R2,R3,R4)
        eg_levels  = [None] * 6   # each entry: (L1,L2,L3,L4)
        for sysex_idx in range(6):
            op_base  = sysex_idx * OP_SIZE
            our_idx  = 5 - sysex_idx
            out_levels[our_idx] = voice[op_base + OP_OUTLEVEL]
            eg_rates[our_idx]   = tuple(voice[op_base + r] for r in OP_RATES)
            eg_levels[our_idx]  = tuple(voice[op_base + l] for l in OP_LEVELS)

        algo_usage[algo] = algo_usage.get(algo, 0) + 1
        lv = " ".join(f"{x:2d}" for x in out_levels)
        print(f"  {v+1:2d}. {name:<10s}  alg={algo:2d}  fb={fb}  "
              f"OP1-6 out-level: {lv}")
        for op in range(6):
            r = eg_rates[op]
            l = eg_levels[op]
            print(f"        OP{op+1}  R={r[0]:2d}/{r[1]:2d}/{r[2]:2d}/{r[3]:2d}"
                  f"   L={l[0]:2d}/{l[1]:2d}/{l[2]:2d}/{l[3]:2d}")

    print()
    used = sorted(algo_usage)
    print("algorithms used by this cartridge (verify these first):")
    print("  " + ", ".join(f"{a}(x{algo_usage[a]})" for a in used))


# ----------------------------------------------------------------------
# Algorithm spec compiler
# ----------------------------------------------------------------------
# Fill one entry per algorithm, 1..32, reading Dexed's diagram directly.
# Format per entry (operators are 1-based here, exactly as Dexed labels them):
#     "carriers": [list of carrier operator numbers]
#     "routes":   [(src, dst), ...]   modulator -> target
#     "fb":       operator number that has the feedback loop
#
# Leave an entry as None until you've verified it against Dexed.
# alg 22 below is the entry we confirmed together (BRASS 1).

SPECS = {n: None for n in range(1, 33)}

SPECS[22] = {
    "carriers": [1, 3, 4, 5],
    "routes":   [(2, 1), (6, 3), (6, 4), (6, 5)],
    "fb":       6,
}

# ----------------------------------------------------------------------


def validate(n, spec):
    errs = []
    carriers = set(spec["carriers"])
    routes = spec["routes"]
    fb = spec["fb"]

    for c in carriers:
        if not (1 <= c <= 6):
            errs.append(f"carrier {c} out of range")
    for (s, d) in routes:
        if not (1 <= s <= 6):
            errs.append(f"route source {s} out of range")
        if not (1 <= d <= 6):
            errs.append(f"route dest {d} out of range")
    if not (1 <= fb <= 6):
        errs.append(f"feedback op {fb} out of range")
    if len(routes) > 5:
        errs.append(f"{len(routes)} routes (max 5 fit in the struct)")

    # every operator must appear somewhere (carrier, or src/dst of a route)
    seen = set(carriers)
    for (s, d) in routes:
        seen.add(s); seen.add(d)
    missing = [op for op in range(1, 7) if op not in seen]
    if missing:
        errs.append(f"operators not referenced anywhere: {missing}")

    return errs


def emit_cpp_entry(n, spec):
    carriers = set(spec["carriers"])
    routes = spec["routes"]
    fb = spec["fb"]

    is_carrier = ["true" if (op + 1) in carriers else "false" for op in range(6)]
    route_strs = [f"{{{s-1},{d-1}}}" for (s, d) in routes]
    while len(route_strs) < 5:
        route_strs.append("{0,0}")

    carrier_list = ", ".join(str(c) for c in sorted(carriers))
    route_desc = " ".join(f"{s}->{d}" for (s, d) in routes)

    return (
        f"    // Algorithm {n}: carriers {carrier_list}; "
        f"{route_desc}; feedback OP{fb}\n"
        f"    {{\n"
        f"        {{ {', '.join(is_carrier)} }},\n"
        f"        {{ {', '.join(route_strs)} }},\n"
        f"        {len(routes)}, {fb-1}\n"
        f"    }},\n"
    )


def emit_placeholder(n):
    return (
        f"    // Algorithm {n}: TODO — UNVERIFIED, read from Dexed's diagram\n"
        f"    {{\n"
        f"        {{ true, false, false, false, false, false }},\n"
        f"        {{ {{0,0}}, {{0,0}}, {{0,0}}, {{0,0}}, {{0,0}} }},\n"
        f"        0, 0\n"
        f"    }},\n"
    )


def generate():
    out = []
    out.append("static const Dx7AlgorithmDef kAlgorithms[32] = {\n")
    unfilled = []
    any_err = False
    for n in range(1, 33):
        spec = SPECS[n]
        if spec is None:
            unfilled.append(n)
            out.append(emit_placeholder(n))
            continue
        errs = validate(n, spec)
        if errs:
            any_err = True
            sys.stderr.write(f"ERROR alg {n}: " + "; ".join(errs) + "\n")
            out.append(emit_placeholder(n))
            continue
        out.append(emit_cpp_entry(n, spec))
    out.append("};\n")

    sys.stdout.write("".join(out))

    if unfilled:
        sys.stderr.write(
            f"\n{len(unfilled)} algorithm(s) still UNVERIFIED: {unfilled}\n"
            "Fill these in SPECS by reading Dexed's diagrams.\n")
    if any_err:
        sys.stderr.write("Validation errors above — fix the SPECS entries.\n")


def main():
    if len(sys.argv) < 2:
        print(__doc__)
        return
    cmd = sys.argv[1]
    if cmd == "dump":
        if len(sys.argv) < 3:
            print("usage: dx7_algo.py dump <file.syx>")
            return
        dump_cartridge(sys.argv[2])
    elif cmd == "gen":
        generate()
    else:
        print(__doc__)


if __name__ == "__main__":
    main()