# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Build Commands

### Prerequisites (Linux)
```bash
sudo apt install libasound2-dev xorg-dev
```

### Configure & Build
```bash
cmake -S . -B build-debug -DCMAKE_BUILD_TYPE=Debug
cmake --build build-debug
```

With direnv active, `CMAKE_GENERATOR=Ninja` is set automatically via `.envrc`.

### Run Examples
Built executables land in `build-debug/examples/<name>/format/exe/`:
```bash
./build-debug/examples/osc/format/exe/osc_exe
./build-debug/examples/rawdaw/format/exe/rawdaw_exe
```

There are no test targets currently defined.

## Architecture

Augr is a **C++20 modular audio/DSP framework** organized as a strict layered library stack:

```
augr-core          → Pure model/data layer — no audio I/O, no GUI
augr-rack          → DSP graph execution on top of core
augr-faust         → Faust DSP adapter (wraps generated Faust .h headers)
augr-volt          → Analog-modular synth modules (VCO, VCF, VCA, ADSR, LFO…)
augr-fm            → FM synthesis (Operator, Envelope, Voice, VoiceBank)
augr-faust-library → Pre-compiled Faust DSP modules
augr-exe           → Standalone-app rack (RtAudio + RtMidi I/O)
augite             → GUI layer (SDL3/ImGui shell, ImNodes editor, VST3 editor)
```

All third-party libraries live in `depot/` (vendored, not committed).

### Core Abstractions

- **`Part`** — root base class; carries `owner_` (parent in ownership tree) and `id_`. Uses the custom `REFLECT_ENABLE(Base...)` macro for runtime type introspection.
- **`Model : Part`** — adds `children_` vector; forms the composite data tree.
- **`Graph : Model`** — adds `Wire` management and `Connect`/`Disconnect` API. `graph_dirty_` triggers execution-order rebuild.
- **`Node : Model`** — has `Inport` and `Outport` `Pin` collections.
- **`Module : Node`** — the core DSP unit. Has typed audio/MIDI pins, `parameters_` (`FloatParameter`/`EnumParameter`), and virtual `Process()`/`ProcessAudio()`. Modules declare UI inside `Create()` via `UiBuilder`.
- **`Rack : Graph`** — singleton DSP graph. Holds `modules_` and `sorted_modules_` (Kahn's topological sort). Thread-safe `pending_actions_` / `pending_update_actions_` queues decouple UI thread from audio thread.
- **`ExeRack : Rack`** — standalone-app rack owning `AudioSystem` (RtAudio) and `MidiSystem` (RtMidi). `ProcessAudio()` is the RtAudio callback.

### Signal Flow

```
MidiSystem callback (MIDI thread) → ExeRack::EnqueueMidiMessage() → pending_actions_

AudioSystem callback (audio thread)
  → ExeRack::ProcessAudio()
      → RebuildExecutionOrder() if graph_dirty_
      → ProcessActions()           ← drains pending MIDI etc.
      → foreach sorted_module: m->Process()
      → write output device buffer
      → ProcessUpdateActions()     ← deferred UI-thread callbacks
```

All UI mutations must go through `Rack::EnqueueAction()` — never write module state directly from the UI thread.

### Pin / Wire System

`Pin` → `PinT<T>` → `OutputT` / `MonoInputT` / `PolyInputT` / `QueueInputT`

- `OutputT` publishes via `Connection` callbacks to all connected inputs.
- `PolyInputT` (audio inputs) mixes multiple sources via `Reduce()` — fan-in supported.
- `QueueInputT` buffers incoming values for batch retrieval (MIDI events).
- `Wire` tracks a `Connection` between two `Pin`s; `Graph` owns the wire list.

### Audio Buffer

`Audio` wraps `AudioImpl`, which holds an `xt::xarray<fy_real>` (xtensor, shape `[channels, frames]`). `fy_real` defaults to `float`, switchable at compile time. Supports `operator+`/`operator+=` for mixing.

### GUI (Augite)

`BaseWindow` (SDL3 + ImGui) → `X11Window`/`Win32Window` → `App` (user-subclassed)

- `RackView` is the ImNodes-based node editor; maps imnodes node IDs to `Widget` instances via `widget_map_`.
- `Widget`/`WidgetT<T>` mirrors the `Model`/`ModelFactoryT<T>` factory pattern for the UI side.
- `WidgetManufacturer` is the runtime singleton factory registry for widgets.

### Factory / Registry Pattern

Both `Model` and `Widget` subtypes use paired macros:
- `DEFINE_MODEL_FACTORY(T, NAME, CATEGORY)` — defines a static factory and accessor (in a `.cpp`).
- `REGISTER_MODEL_FACTORY(T)` — registers with `ModelManufacturer::singleton()` at startup.

This enables dynamic module instantiation by name/type at runtime.

### Faust Integration

Faust `.dsp` files are pre-compiled to C++ headers (e.g., `osc_dsp.h`). Subclass both the generated `*Dsp` class and `FaustDsp`. `FaustDsp::Create()` instantiates a `FaustDspUi`, which translates Faust UI callbacks (`addHorizontalSlider`, etc.) into augr model nodes (`HSlider`, `Button`, `VBox`). Regenerating `.h` from `.dsp` is a manual step using the Faust compiler in `depot/faust/`.

## Key Conventions

- **C++20** throughout; `cmake/Standard.cmake` enforces `std=c++20`, no extensions.
- **`fy_real`** is the universal sample type (`float` by default).
- **Header-only modules** where feasible — VCO, Mixer, etc. have all DSP in the `.h`.
- **Platform selection**: edit `cmake/Config.cmake` to switch `SHELL_PLATFORM` between `X11` and `Windows`.
- **`NOMINMAX`** is always defined on targets (Windows compatibility).
- **clang-format**: LLVM base, 4-space indent, no namespace indentation, access modifiers at `-4`.
- **Out-of-source build**: conventional build directory is `build-debug/`.
