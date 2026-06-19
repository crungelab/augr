#!/usr/bin/env python3
"""
dx7_dump.py — read a .syx cartridge and report all per-patch and
per-operator parameters from the packed 32-voice bulk dump format.

Usage:
    python3 dx7_dump.py [--linear | --tabular] [--machine | --human] <file.syx>

Layout (default: --linear):
    --linear   one labeled field per line, easy to read/grep
    --tabular  compact operator grid, one row per operator

Format (default: --machine):
    --machine  raw integer values exactly as stored in the SysEx bytes
    --human    decoded values: detune as signed ±7, break point as note
               name, curves/LFO wave as strings, osc mode as ratio/fixed
"""

import sys

# ----------------------------------------------------------------------
# Packed-voice byte offsets (section F of the DX7 MIDI Data Format Sheet)
# All offsets are operator-relative within the 17-byte operator block.
# Operators are stored OP6..OP1 in the file; we reverse to OP1..OP6.
# ----------------------------------------------------------------------
HEADER      = 6
VOICE_SIZE  = 128
OP_SIZE     = 17        # packed operator stride

# Operator-relative byte offsets
OP_EG_R1      = 0       # EG rate 1           0-99
OP_EG_R2      = 1       # EG rate 2           0-99
OP_EG_R3      = 2       # EG rate 3           0-99
OP_EG_R4      = 3       # EG rate 4           0-99
OP_EG_L1      = 4       # EG level 1          0-99
OP_EG_L2      = 5       # EG level 2          0-99
OP_EG_L3      = 6       # EG level 3          0-99
OP_EG_L4      = 7       # EG level 4          0-99
OP_KBD_BRK    = 8       # kbd level scl break point 0-99 (C3=0x27)
OP_KBD_LD     = 9       # kbd level scl left depth  0-99
OP_KBD_RD     = 10      # kbd level scl right depth 0-99
OP_KBD_CURVES = 11      # bits [3:2]=RC, bits [1:0]=LC  (curve 0-3)
OP_DET_RS     = 12      # bits [6:3]=DET (0-14, centre=7), bits [2:0]=RS (0-7)
OP_KVS_AMS    = 13      # bits [4:2]=KVS (0-7), bits [1:0]=AMS (0-3)
OP_OUTLEVEL   = 14      # output level        0-99
OP_FC_MODE    = 15      # bits [5:1]=FC (0-31), bit [0]=mode (0=ratio 1=fixed)
OP_FF         = 16      # freq fine           0-99

# Voice-level offsets
OFF_PITCH_EG      = 102  # 8 bytes: R1 R2 R3 R4 L1 L2 L3 L4
OFF_ALGO          = 110  # 0-31
OFF_FB_SYNC       = 111  # bits [5:3]=feedback 0-7, bit [0]=osc key sync
OFF_LFO_SPEED     = 112  # 0-99
OFF_LFO_DELAY     = 113  # 0-99
OFF_LFO_PITCH     = 114  # lfo pitch mod depth 0-99
OFF_LFO_AMP       = 115  # lfo amp mod depth 0-99
OFF_LFO_WAVE_SYNC = 116  # bits [3:1]=wave 0-5, bit [0]=lfo sync
OFF_PMS_TRANS     = 117  # bits [6:3]=pitch mod sens 0-7, bits [2:0]=transpose 0-48
OFF_NAME          = 118
NAME_LEN          = 10

CURVE_NAMES    = ['-LIN', '-EXP', '+EXP', '+LIN']
LFO_WAVE_NAMES = ['TRI', 'SAW-DN', 'SAW-UP', 'SQR', 'SIN', 'S/H']


def _fix_char(b):
    return chr(b) if 32 <= b < 127 else '_'


def note_name(midi_note):
    """Break-point raw value (0-99, C3=0x27=39) → note name string."""
    names = ['C','C#','D','D#','E','F','F#','G','G#','A','A#','B']
    octave = (midi_note // 12) - 2
    return f"{names[midi_note % 12]}{octave}"


# ----------------------------------------------------------------------
# Parse — always stores raw values; human rendering happens at print time
# ----------------------------------------------------------------------
def parse_op(voice, sysex_idx):
    op_base = sysex_idx * OP_SIZE
    b11 = voice[op_base + OP_KBD_CURVES]
    b12 = voice[op_base + OP_DET_RS]
    b13 = voice[op_base + OP_KVS_AMS]
    b15 = voice[op_base + OP_FC_MODE]

    return {
        'num':  6 - sysex_idx,              # OP1..OP6
        'out':  voice[op_base + OP_OUTLEVEL],
        'mode': b15 & 0x01,                  # raw: 0=ratio 1=fixed
        'fc':   (b15 >> 1) & 0x1F,          # 0-31
        'ff':   voice[op_base + OP_FF],      # 0-99
        'det':  (b12 >> 3) & 0x0F,          # raw 0-14, centre=7
        'rs':   b12 & 0x07,                  # 0-7
        'kvs':  (b13 >> 2) & 0x07,          # 0-7
        'ams':  b13 & 0x03,                  # 0-3
        'brk':  voice[op_base + OP_KBD_BRK],# raw 0-99
        'ld':   voice[op_base + OP_KBD_LD], # 0-99
        'rd':   voice[op_base + OP_KBD_RD], # 0-99
        'lc':   b11 & 0x03,                  # raw 0-3
        'rc':   (b11 >> 2) & 0x03,          # raw 0-3
        'eg_r': tuple(voice[op_base + r] for r in (OP_EG_R1, OP_EG_R2, OP_EG_R3, OP_EG_R4)),
        'eg_l': tuple(voice[op_base + l] for l in (OP_EG_L1, OP_EG_L2, OP_EG_L3, OP_EG_L4)),
    }


def parse_voice(data, v):
    base  = HEADER + v * VOICE_SIZE
    voice = data[base:base + VOICE_SIZE]
    if len(voice) < VOICE_SIZE:
        return None

    peg = voice[OFF_PITCH_EG:OFF_PITCH_EG + 8]

    return {
        'index':     v + 1,
        'name':      ''.join(_fix_char(b) for b in voice[OFF_NAME:OFF_NAME + NAME_LEN]).rstrip(),
        'algo':      (voice[OFF_ALGO] & 0x1F) + 1,   # 1-based
        'fb':        (voice[OFF_FB_SYNC] >> 3) & 0x07,
        'osc_sync':  voice[OFF_FB_SYNC] & 0x01,
        'lfo_speed': voice[OFF_LFO_SPEED],
        'lfo_delay': voice[OFF_LFO_DELAY],
        'lfo_pitch': voice[OFF_LFO_PITCH],
        'lfo_amp':   voice[OFF_LFO_AMP],
        'lfo_wave':  (voice[OFF_LFO_WAVE_SYNC] >> 1) & 0x07,  # raw 0-5
        'lfo_sync':  voice[OFF_LFO_WAVE_SYNC] & 0x01,
        'pms':       (voice[OFF_PMS_TRANS] >> 3) & 0x07,
        'transpose': voice[OFF_PMS_TRANS] & 0x07,
        'peg_r':     tuple(peg[0:4]),
        'peg_l':     tuple(peg[4:8]),
        'ops':       [parse_op(voice, i) for i in range(6)],
    }


# ----------------------------------------------------------------------
# Format helpers — machine keeps raw ints, human decodes to strings
# ----------------------------------------------------------------------
def fmt_det(raw, human):
    return f"{raw - 7:+d}" if human else str(raw)

def fmt_brk(raw, human):
    return note_name(raw) if human else str(raw)

def fmt_curve(raw, human):
    return CURVE_NAMES[raw] if human else str(raw)

def fmt_mode(raw, human):
    return ('fixed' if raw else 'ratio') if human else str(raw)

def fmt_lfo_wave(raw, human):
    return (LFO_WAVE_NAMES[raw] if raw < len(LFO_WAVE_NAMES) else str(raw)) if human else str(raw)


# ----------------------------------------------------------------------
# Linear layout
# ----------------------------------------------------------------------
def print_linear(v, human):
    HR = '─' * 60
    print(HR)
    print(f"  {v['index']:2d}. {v['name']}")
    print(f"      algorithm={v['algo']}  feedback={v['fb']}  osc_sync={v['osc_sync']}")
    r, l = v['peg_r'], v['peg_l']
    print(f"      pitch_EG  rates={r[0]}/{r[1]}/{r[2]}/{r[3]}"
          f"  levels={l[0]}/{l[1]}/{l[2]}/{l[3]}")
    print(f"      LFO  wave={fmt_lfo_wave(v['lfo_wave'], human)}"
          f"  speed={v['lfo_speed']}  delay={v['lfo_delay']}  sync={v['lfo_sync']}")
    print(f"           pitch_depth={v['lfo_pitch']}  amp_depth={v['lfo_amp']}"
          f"  pms={v['pms']}  transpose={v['transpose']}")
    print()

    for op in v['ops']:
        print(f"      OP{op['num']}")
        print(f"        output_level={op['out']}"
              f"  mode={fmt_mode(op['mode'], human)}"
              f"  coarse={op['fc']}  fine={op['ff']}"
              f"  detune={fmt_det(op['det'], human)}")
        print(f"        rate_scale={op['rs']}  kvs={op['kvs']}  ams={op['ams']}")
        print(f"        kbd_scl  break={fmt_brk(op['brk'], human)}"
              f"  left_depth={op['ld']}  left_curve={fmt_curve(op['lc'], human)}"
              f"  right_depth={op['rd']}  right_curve={fmt_curve(op['rc'], human)}")
        r, l = op['eg_r'], op['eg_l']
        print(f"        EG  rates={r[0]}/{r[1]}/{r[2]}/{r[3]}"
              f"  levels={l[0]}/{l[1]}/{l[2]}/{l[3]}")
        print()


# ----------------------------------------------------------------------
# Tabular layout
# ----------------------------------------------------------------------
def print_tabular(v, human):
    HR = '─' * 60
    print(HR)
    print(f"  {v['index']:2d}. {v['name']}"
          f"   alg={v['algo']}  fb={v['fb']}  osc_sync={v['osc_sync']}")
    r, l = v['peg_r'], v['peg_l']
    print(f"      pitch_EG  R={r[0]}/{r[1]}/{r[2]}/{r[3]}"
          f"   L={l[0]}/{l[1]}/{l[2]}/{l[3]}")
    print(f"      LFO  wave={fmt_lfo_wave(v['lfo_wave'], human)}"
          f"  spd={v['lfo_speed']}  dly={v['lfo_delay']}  sync={v['lfo_sync']}"
          f"  ptch={v['lfo_pitch']}  amp={v['lfo_amp']}"
          f"  pms={v['pms']}  trans={v['transpose']}")
    print()

    # Column widths depend on format: human curves are 4 chars, machine is 1
    crv_w = 4 if human else 1
    det_w = 3 if human else 2
    brk_w = 4 if human else 2
    mode_w = 5 if human else 1

    print(f"      {'':4s}  {'out':>3s}  {'mode':>{mode_w}s}"
          f"  {'fc':>2s}  {'ff':>2s}  {'det':>{det_w}s}"
          f"  {'rs':>2s}  {'kvs':>3s}  {'ams':>3s}"
          f"  {'brk':>{brk_w}s}  {'ldp':>3s}  {'lcrv':>{crv_w}s}"
          f"  {'rdp':>3s}  {'rcrv':>{crv_w}s}"
          f"  {'R1':>2s} {'R2':>2s} {'R3':>2s} {'R4':>2s}"
          f"  {'L1':>2s} {'L2':>2s} {'L3':>2s} {'L4':>2s}")

    for op in v['ops']:
        r, l = op['eg_r'], op['eg_l']
        print(f"      OP{op['num']}  {op['out']:>3d}"
              f"  {fmt_mode(op['mode'], human):>{mode_w}s}"
              f"  {op['fc']:>2d}  {op['ff']:>2d}"
              f"  {fmt_det(op['det'], human):>{det_w}s}"
              f"  {op['rs']:>2d}  {op['kvs']:>3d}  {op['ams']:>3d}"
              f"  {fmt_brk(op['brk'], human):>{brk_w}s}"
              f"  {op['ld']:>3d}  {fmt_curve(op['lc'], human):>{crv_w}s}"
              f"  {op['rd']:>3d}  {fmt_curve(op['rc'], human):>{crv_w}s}"
              f"  {r[0]:>2d} {r[1]:>2d} {r[2]:>2d} {r[3]:>2d}"
              f"  {l[0]:>2d} {l[1]:>2d} {l[2]:>2d} {l[3]:>2d}")
    print()


# ----------------------------------------------------------------------
# Top-level
# ----------------------------------------------------------------------
def dump_cartridge(path, layout, human):
    with open(path, 'rb') as f:
        data = f.read()

    print(f"file: {path}  ({len(data)} bytes)")
    if len(data) < HEADER + VOICE_SIZE * 32 + 2:
        print("  WARNING: smaller than a 32-voice cartridge")
    if data[0] != 0xF0 or data[1] != 0x43 or data[3] != 0x09:
        print("  WARNING: header does not look like a DX7 32-voice dump")
    print()

    printer    = print_tabular if layout == 'tabular' else print_linear
    algo_usage = {}

    for v in range(32):
        voice = parse_voice(data, v)
        if voice is None:
            break
        algo_usage[voice['algo']] = algo_usage.get(voice['algo'], 0) + 1
        printer(voice, human)

    print('─' * 60)
    used = sorted(algo_usage)
    print("algorithms used by this cartridge:")
    print("  " + ", ".join(f"{a}(x{algo_usage[a]})" for a in used))


def main():
    args   = sys.argv[1:]
    layout = 'linear'
    human  = False

    remaining = []
    for arg in args:
        if arg == '--linear':
            layout = 'linear'
        elif arg == '--tabular':
            layout = 'tabular'
        elif arg == '--machine':
            human = False
        elif arg == '--human':
            human = True
        elif arg.startswith('--'):
            print(f"unknown option: {arg}", file=sys.stderr)
            print(__doc__)
            sys.exit(1)
        else:
            remaining.append(arg)

    if len(remaining) != 1:
        print(__doc__)
        sys.exit(1)

    dump_cartridge(remaining[0], layout, human)


if __name__ == "__main__":
    main()