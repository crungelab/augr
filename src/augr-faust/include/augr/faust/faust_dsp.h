#pragma once

#include <augr/rack/module/dsp.h>

class UI;

namespace augr {

class FaustDsp : public Dsp {
public:
    virtual ~FaustDsp() = default;
    void Create(Model *parent = nullptr) override;
    void CreatePins() override;
    void CreateControls() override;
    void Process() override;

    // Faust interface
    virtual int getNumInputs() = 0;
    virtual int getNumOutputs() = 0;
    virtual void buildUserInterface(UI *ui) = 0;
    virtual void init(int sample_rate) = 0;
    virtual void compute(int len, fy_real **inputs, fy_real **outputs) = 0;
    virtual void control() {}

    REFLECT_ENABLE(Dsp)
};

} // namespace augr