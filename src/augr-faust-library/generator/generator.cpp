#include "../library.h"

#include "church_organ_dsp.h"
#include "osc_dsp.h"
#include "sawtooth_lab_dsp.h"
#include "virtual_analog_dsp.h"
#include "virtual_analog_lab_dsp.h"

using namespace augr;

// Generator

class ChurchOrganDspImpl final : public ChurchOrganDsp {
    REFLECT_ENABLE(FaustDsp)
};
DEFINE_MODEL_FACTORY(ChurchOrganDspImpl, "Church Organ", "Generator")

class OscDspImpl final : public OscDsp {
    REFLECT_ENABLE(FaustDsp)
};
DEFINE_MODEL_FACTORY(OscDspImpl, "Oscillator", "Generator")

class SawtoothLabDspImpl final : public SawtoothLabDsp {
    REFLECT_ENABLE(FaustDsp)
};
DEFINE_MODEL_FACTORY(SawtoothLabDspImpl, "Sawtooth Lab", "Generator")

class VirtualAnalogDspImpl final : public VirtualAnalogDsp {
    REFLECT_ENABLE(FaustDsp)
};
DEFINE_MODEL_FACTORY(VirtualAnalogDspImpl, "Virtual Analog", "Generator")


class VirtualAnalogLabDspImpl final : public VirtualAnalogLabDsp {
    REFLECT_ENABLE(FaustDsp)
};
DEFINE_MODEL_FACTORY(VirtualAnalogLabDspImpl, "Virtual Analog Lab", "Generator")

void InitFaustDspLibrary_Generator() {
    REGISTER_MODEL_FACTORY(ChurchOrganDspImpl);
    REGISTER_MODEL_FACTORY(OscDspImpl);
    REGISTER_MODEL_FACTORY(SawtoothLabDspImpl);
    REGISTER_MODEL_FACTORY(VirtualAnalogDspImpl);
    REGISTER_MODEL_FACTORY(VirtualAnalogLabDspImpl);
}
