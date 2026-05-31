#pragma once
#include "device.h"

namespace augr {

class AudioDevice : public Device {
    REFLECT_ENABLE(Device)
};

class AudioInputDevice : public AudioDevice {
public:
    virtual ~AudioInputDevice() {}
    void Create() override;
    void CreatePins() override;
    REFLECT_ENABLE(AudioDevice)
};

class AudioOutputDevice : public AudioDevice {
public:
    virtual ~AudioOutputDevice() {}
    void Create() override;
    void CreatePins() override;
    REFLECT_ENABLE(AudioDevice)
};

} // namespace augr