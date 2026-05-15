#include "../library.h"

#include "phaser_dsp.h"
#include "flanger_dsp.h"

using namespace augr;

// Phasing

class FlangerDspImpl final : public FlangerDsp {
    REFLECT_ENABLE(FaustDsp)
};
DEFINE_MODULE(FlangerDspImpl, "Flanger", "Effect")

class PhaserDspImpl final : public PhaserDsp {
    REFLECT_ENABLE(FaustDsp)
};
DEFINE_MODULE(PhaserDspImpl, "Phaser", "Effect")

void InitFaustDspLibrary_Phasing() {
    REGISTER_MODULE(FlangerDspImpl);
    REGISTER_MODULE(PhaserDspImpl);
}
