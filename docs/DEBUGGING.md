## VST3SDK editorhost Debugging

```bash
cd depot/vst3sdk/cmake-build/bin/Debug

./editorhost /home/kurt/Dev/augr/build-debug/VST3/Debug/panner_x11_vst3.vst3

gdb --args ./editorhost /home/kurt/Dev/augr/build-debug/VST3/Debug/panner_x11_vst3.vst3
```

```bash
cd depot/vst3sdk/cmake-build/bin/Debug

./editorhost ~/.vst3/panner_x11_vst3.vst3

gdb --args ./editorhost ~/.vst3/panner_x11_vst3.vst3
```

### For Comparison

```bash
./editorhost ~/.vst3/panner.vst3
```

## Reaper Debugging
gdb --args ~/opt/REAPER/reaper

