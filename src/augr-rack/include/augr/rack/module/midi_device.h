#pragma once
#include "device.h"

namespace augr {

class MidiDevice : public Device {
    REFLECT_ENABLE(Device)
};

class MidiInputDevice : public MidiDevice {
public:
    virtual ~MidiInputDevice() {}
    void Create() override;
    void CreatePins() override;
    REFLECT_ENABLE(MidiDevice)
};

class MidiOutputDevice : public MidiDevice {
public:
    virtual ~MidiOutputDevice() {}
    void Create() override;
    void CreatePins() override;
    REFLECT_ENABLE(MidiDevice)
};

} // namespace augr