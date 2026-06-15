#pragma once
#include "io.h"

namespace augr {

class AudioIo : public Io {
    REFLECT_ENABLE(Io)
};

class AudioInputModule : public AudioIo {
public:
    virtual ~AudioInputModule() {}
    void OnCreate() override;
    void CreatePins() override;
    void Process() override;
    REFLECT_ENABLE(AudioIo)
};

class AudioOutputModule : public AudioIo {
public:
    virtual ~AudioOutputModule() {}
    void OnCreate() override;
    void CreatePins() override;
    void Process() override;
    REFLECT_ENABLE(AudioIo)
};

} // namespace augr