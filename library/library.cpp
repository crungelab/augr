#include "library.h"

#include "freeverb_dsp.h"
#include "osc_dsp.h"

using namespace augr;

class OscDspImpl final : public OscDsp {
    REFLECT_ENABLE(FaustDsp)
};
DEFINE_MODEL_FACTORY(OscDspImpl, "Oscillator", "Generator")

class FreeverbDspImpl final : public FreeverbDsp {
    REFLECT_ENABLE(FaustDsp)
};
DEFINE_MODEL_FACTORY(FreeverbDspImpl, "Freeverb", "Reverb")


void InitFaustDspLibrary() {
    InitFaustDspLibrary_Analysis();
    InitFaustDspLibrary_GameAudio();
    InitFaustDspLibrary_Phasing();
    InitFaustDspLibrary_PhysicalModeling();
    InitFaustDspLibrary_PhysicalModeling_FaustStk();
    REGISTER_MODEL_FACTORY(OscDspImpl);
    REGISTER_MODEL_FACTORY(FreeverbDspImpl);
}
