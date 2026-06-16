#!/usr/bin/env python3
"""
parse_orc.py — extract DX7 algorithm routing/carrier/feedback data directly
from the Csound .orc wiring block (the section after ';---Algorithm N---').

This is ground truth: each .orc file's final "output:" block hardcodes that
algorithm's exact signal routing using three patterns, applied consistently
across all 32 files:

    aNsig oscili kNgate,iNhz,1
        -> operator N has no incoming phase modulation (leaf).

    aNsig tablei aNphs + aM sig [+ aP sig ...], 1,1,0,1
        -> operator N is phase-modulated by M (and P, ...): route M->N.

    aNsig tablei aNphs + aNsig*(K*ifeed), 1,1,0,1
        -> operator N self-feeds-back, with scalar K.

    out (aXsig + aYsig + ...) * ipkamp
        -> X, Y, ... are the carriers.

Usage:
    python3 parse_orc.py dx701.orc dx702.orc ... dx732.orc
    python3 parse_orc.py /path/to/dir/*.orc
"""

import re
import sys
import os


def extract_wiring_block(text):
    """Return the text from the per-algorithm marker comment to 'endin'."""
    m = re.search(r';-+\s*Algorithm\s+(\d+)\s*-+;?', text)
    if not m:
        raise ValueError("could not find ';---Algorithm N---' marker")
    algo_num = int(m.group(1))
    start = m.end()
    end_m = re.search(r'\bendin\b', text[start:])
    if not end_m:
        raise ValueError("could not find 'endin' after algorithm marker")
    block = text[start:start + end_m.start()]
    return algo_num, block


def parse_block(block):
    """
    Parse the wiring block into:
        carriers: sorted list of operator numbers (1-based)
        routes:   list of (src, dst) 1-based modulator->target pairs
        feedback_op: operator number with self-feedback, or None
        feedback_scale: the K constant in K*ifeed, or None
    """
    routes = []
    feedback_op = None
    feedback_scale = None

    # aNsig tablei aNphs + aNsig*(K*ifeed) ...   -- self feedback, explicit scale.
    for m in re.finditer(
        r'a(\d)sig\s+tablei\s+a\1phs\s*\+\s*a\1sig\s*\*\s*\(\s*([\d.]+)\s*\*\s*ifeed\s*\)',
        block):
        feedback_op = int(m.group(1))
        feedback_scale = float(m.group(2))

    # aNsig tablei aNphs + aNsig*ifeed ...        -- self feedback, no scalar
    # (implicit scale of 1.0). Only matches if the parenthesized form above
    # didn't already find this operator's feedback.
    if feedback_op is None:
        for m in re.finditer(
            r'a(\d)sig\s+tablei\s+a\1phs\s*\+\s*a\1sig\s*\*\s*ifeed\b',
            block):
            feedback_op = int(m.group(1))
            feedback_scale = 1.0

    # aNsig tablei aNphs + aMsig [+ aPsig ...] , ...   -- phase modulation routes.
    # Exclude the feedback form (aNsig*(...)) by requiring the summed terms to
    # be plain "a<digit>sig" tokens, not "a<digit>sig*(...)".
    for m in re.finditer(
        r'a(\d)sig\s+tablei\s+a\1phs\s*\+\s*([a\d\s\+sig]+?)\s*,\s*1,1,0,1',
        block):
        dst = int(m.group(1))
        rhs = m.group(2)
        # Skip if this is actually the feedback form for this operator.
        if re.match(r'^a' + str(dst) + r'sig\s*\*', rhs.strip()):
            continue
        srcs = re.findall(r'a(\d)sig', rhs)
        for s in srcs:
            src = int(s)
            if src == dst:
                continue  # already captured as feedback above
            routes.append((src, dst))

    # out (aXsig + aYsig + ...) * ipkamp   -- multiple carriers, parenthesized.
    # out aXsig * ipkamp                   -- single carrier, no parens.
    out_m = re.search(r'\bout\s+\(([^)]+)\)\s*\*\s*ipkamp', block)
    if out_m:
        out_terms = out_m.group(1)
    else:
        out_m = re.search(r'\bout\s+(a\d sig)\s*\*\s*ipkamp'.replace(' ', ''), block)
        if not out_m:
            raise ValueError("could not find 'out (...) * ipkamp' or "
                             "'out aNsig * ipkamp' line")
        out_terms = out_m.group(1)
    carriers = sorted(int(x) for x in re.findall(r'a(\d)sig', out_terms))

    # De-duplicate routes while preserving first-seen order.
    seen = set()
    deduped = []
    for r in routes:
        if r not in seen:
            seen.add(r)
            deduped.append(r)

    return {
        "carriers": carriers,
        "routes": deduped,
        "fb": feedback_op,
        "fb_scale": feedback_scale,
    }


def validate(algo_num, spec):
    errs = []
    carriers = set(spec["carriers"])
    routes = spec["routes"]

    for c in carriers:
        if not (1 <= c <= 6):
            errs.append(f"carrier {c} out of range")
    for (s, d) in routes:
        if not (1 <= s <= 6) or not (1 <= d <= 6):
            errs.append(f"route ({s},{d}) out of range")
    if len(routes) > 5:
        errs.append(f"{len(routes)} routes found (max 5 fit in Dx7AlgorithmDef)")
    if spec["fb"] is None:
        pass  # legitimate — several DX7 algorithms have no feedback at all

    seen = set(carriers)
    for (s, d) in routes:
        seen.add(s); seen.add(d)
    if spec["fb"] is not None:
        seen.add(spec["fb"])
    missing = [op for op in range(1, 7) if op not in seen]
    if missing:
        errs.append(f"operators never referenced: {missing}")

    return errs


def emit_cpp_entry(algo_num, spec):
    carriers = set(spec["carriers"])
    routes = spec["routes"]
    fb = spec["fb"]

    is_carrier = ["true" if op in carriers else "false" for op in range(1, 7)]
    route_strs = [f"{{{s-1},{d-1}}}" for (s, d) in routes]
    while len(route_strs) < 5:
        route_strs.append("{0,0}")

    carrier_list = ", ".join(str(c) for c in sorted(carriers))
    route_desc = " ".join(f"{s}->{d}" for (s, d) in routes)
    fb_desc = f"OP{fb} (scale {spec['fb_scale']})" if fb else "none"
    fb_op_emitted = (fb - 1) if fb else -1
    fb_scale_emitted = spec["fb_scale"] if spec["fb_scale"] is not None else 0.0

    return (
        f"    // Algorithm {algo_num}: carriers {carrier_list}; "
        f"{route_desc}; feedback {fb_desc}\n"
        f"    {{\n"
        f"        {{ {', '.join(is_carrier)} }},\n"
        f"        {{ {{ {', '.join(route_strs)} }} }},\n"
        f"        {len(routes)}, {fb_op_emitted}, {fb_scale_emitted}f\n"
        f"    }},\n"
    )


def main():
    paths = sys.argv[1:]
    if not paths:
        print(__doc__)
        return

    results = {}
    any_err = False

    for path in paths:
        with open(path, 'r', errors='replace') as f:
            text = f.read()
        try:
            algo_num, block = extract_wiring_block(text)
            spec = parse_block(block)
        except ValueError as e:
            sys.stderr.write(f"FAILED to parse {os.path.basename(path)}: {e}\n")
            any_err = True
            continue

        errs = validate(algo_num, spec)
        if errs:
            sys.stderr.write(f"VALIDATION ERRORS in algorithm {algo_num} "
                             f"({os.path.basename(path)}): {'; '.join(errs)}\n")
            any_err = True

        if algo_num in results:
            sys.stderr.write(f"WARNING: algorithm {algo_num} parsed from "
                             f"multiple files (duplicate?)\n")
        results[algo_num] = spec

    print("static const Dx7AlgorithmDef kAlgorithms[32] = {")
    for n in range(1, 33):
        if n in results:
            sys.stdout.write(emit_cpp_entry(n, results[n]))
        else:
            sys.stderr.write(f"MISSING algorithm {n} — no input file covered it\n")
            sys.stdout.write(
                f"    // Algorithm {n}: MISSING — no source file provided\n"
                f"    {{ {{true,false,false,false,false,false}}, "
                f"{{{{0,0}},{{0,0}},{{0,0}},{{0,0}},{{0,0}}}}, 0, -1, 0.0f }},\n")
            any_err = True
    print("};")

    if any_err:
        sys.stderr.write("\nOne or more issues above — review before trusting "
                         "this table.\n")
    else:
        sys.stderr.write(f"\nAll {len(results)} algorithms parsed and "
                         f"validated cleanly.\n")


if __name__ == "__main__":
    main()