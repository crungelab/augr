#include "../library.h"

#include "phaser_dsp.h"
#include "flanger_dsp.h"

using namespace augr;

// Game Audio

class FlangerDspImpl final : public FlangerDsp {
    REFLECT_ENABLE(FaustDsp)
};
DEFINE_MODEL_FACTORY(FlangerDspImpl, "Flanger", "Effect")

class PhaserDspImpl final : public PhaserDsp {
    REFLECT_ENABLE(FaustDsp)
};
DEFINE_MODEL_FACTORY(PhaserDspImpl, "Phaser", "Effect")

void InitFaustDspLibrary_Phasing() {
    REGISTER_MODEL_FACTORY(FlangerDspImpl);
    REGISTER_MODEL_FACTORY(PhaserDspImpl);
}
