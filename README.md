
# Augr :control_knobs:

- Audio Graph
- Basic VST3 Support

[FAUST](https://faust.grame.fr/)

[Dear ImGui](https://github.com/ocornut/imgui)

## Getting Started

```bash
git clone --recursive https://github.com/kfields/augr
cd augr

mkdir build-debug
cd build-debug
cmake -G "Visual Studio 17 2022" -DCMAKE_BUILD_TYPE=Debug ..
cmake -DCMAKE_BUILD_TYPE=Debug ..
```

## Linux Dev Requirements
```bash
#RtAudio
sudo apt install libasound2-dev
#Glfw
sudo apt install xorg-dev
```

## Configure

```bash
cmake -S . -B build-debug -DCMAKE_BUILD_TYPE=Debug
```

## Build

```bash
cmake --build build-debug
```