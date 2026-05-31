#pragma once
#include <augr/rack/module/io.h>
#include <augr/rack/voltage_pin.h>

namespace augr {

class CvIo : public Io {
public:
    VoltageInput *cv_in_ = nullptr;
    VoltageOutput *cv_out_ = nullptr;
    REFLECT_ENABLE(Io)
};

class CvInputModule : public CvIo {
public:
    virtual ~CvInputModule() {}
    void Create() override;
    void CreatePins() override;
    void Process() override;
    // Data members
    REFLECT_ENABLE(CvIo)
};

class CvOutputModule : public CvIo {
public:
    virtual ~CvOutputModule() {}
    void Create() override;
    void CreatePins() override;
    void Process() override;
    REFLECT_ENABLE(CvIo)
};

} // namespace augr