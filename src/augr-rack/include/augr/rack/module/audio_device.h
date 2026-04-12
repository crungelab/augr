#pragma once
#include "device.h"

namespace augr {

class AudioDevice : public Device
{
  REFLECT_ENABLE(Device)
};

class AudioInputDevice : public AudioDevice
{
public:
  virtual ~AudioInputDevice() {}
  bool Create(Part &owner) override;
  REFLECT_ENABLE(AudioDevice)
};

class AudioOutputDevice : public AudioDevice
{
public:
  virtual ~AudioOutputDevice() {}
  bool Create(Part &owner) override;
  REFLECT_ENABLE(AudioDevice)
};

} // namespace augr