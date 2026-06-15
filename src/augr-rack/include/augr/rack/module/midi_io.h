#pragma once
#include "io.h"

namespace augr {

class MidiIo : public Io {
    REFLECT_ENABLE(Io)
};

class MidiInputModule : public MidiIo {
public:
    virtual ~MidiInputModule() {}
    void OnCreate() override;
    void CreatePins() override;
    void Process() override;
    REFLECT_ENABLE(MidiIo)
};

class MidiOutputModule : public MidiIo {
public:
    virtual ~MidiOutputModule() {}
    void OnCreate() override;
    void CreatePins() override;
    void Process() override;
    REFLECT_ENABLE(MidiIo)
};

} // namespace augr