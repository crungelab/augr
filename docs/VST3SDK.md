## Build for Linux
```bash
sudo apt install libgtkmm-3.0-dev
cd depot/vst3sdk
cmake --build cmake-build
```

## Generate UID for Components
```bash
uuidgen | tr -d '-' | sed -E 's/(........)(........)(........)(........)/0x\1, 0x\2, 0x\3, 0x\4/'
```