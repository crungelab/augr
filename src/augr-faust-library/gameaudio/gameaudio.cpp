#include "../library.h"

#include "bubble_dsp.h"
#include "rain_dsp.h"
#include "thunder_dsp.h"

using namespace augr;

// Game Audio

class BubbleDspImpl final : public BubbleDsp {
    REFLECT_ENABLE(FaustDsp)
};
DEFINE_MODEL_FACTORY(BubbleDspImpl, "Bubble", "Generator")

class RainDspImpl final : public RainDsp {
    REFLECT_ENABLE(FaustDsp)
};
DEFINE_MODEL_FACTORY(RainDspImpl, "Rain", "Generator")

class ThunderDspImpl final : public ThunderDsp {
    REFLECT_ENABLE(FaustDsp)
};
DEFINE_MODEL_FACTORY(ThunderDspImpl, "Thunder", "Generator")

void InitFaustDspLibrary_GameAudio() {
    REGISTER_MODEL_FACTORY(BubbleDspImpl);
    REGISTER_MODEL_FACTORY(RainDspImpl);
    REGISTER_MODEL_FACTORY(ThunderDspImpl);
}
