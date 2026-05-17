#pragma once
#include "device.h"

namespace augr {

class MidiDevice : public Device
{
  REFLECT_ENABLE(Device)
};

class MidiInputDevice : public MidiDevice
{
public:
  virtual ~MidiInputDevice() {}
  void Create(Model *parent = nullptr) override;
  void CreatePins() override;
  REFLECT_ENABLE(MidiDevice)
};

class MidiOutputDevice : public MidiDevice
{
public:
  virtual ~MidiOutputDevice() {}
  void Create(Model *parent = nullptr) override;
  void CreatePins() override;
  REFLECT_ENABLE(MidiDevice)
};

} // namespace augr