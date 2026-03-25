#include <augr/core/rack/model_manufacturer.h>

#include <augr/core/rack/module/faust_dsp.h>
#include <augr/core/rack/module/faust_dsp_ui.h>
#include <augr/core/rack/module/module.h>

#include "dbmeter_dsp.h"
#include "flanger_dsp.h"
#include "freeverb_dsp.h"
#include "frenchbell_dsp.h"
#include "osc_dsp.h"
#include "phaser_dsp.h"

using namespace augr;

// Analysis

class DbMeterDspImpl : public DbMeterDsp {
public:
    REFLECT_ENABLE(FaustDsp)
};
DEFINE_MODEL_FACTORY(DbMeterDspImpl, "Db Meter", "Analysis")

class FrenchBellDspImpl : public FrenchBellDsp {
public:
    REFLECT_ENABLE(FaustDsp)
};
DEFINE_MODEL_FACTORY(FrenchBellDspImpl, "French Bell", "Instrument")

class OscDspImpl final : public OscDsp {
    REFLECT_ENABLE(FaustDsp)
};
DEFINE_MODEL_FACTORY(OscDspImpl, "Oscillator", "Generator")

class FreeverbDspImpl final : public FreeverbDsp {
    REFLECT_ENABLE(FaustDsp)
};
DEFINE_MODEL_FACTORY(FreeverbDspImpl, "Freeverb", "Reverb")

class FlangerDspImpl final : public FlangerDsp {
    REFLECT_ENABLE(FaustDsp)
};
DEFINE_MODEL_FACTORY(FlangerDspImpl, "Flanger", "Effect")

class PhaserDspImpl final : public PhaserDsp {
    REFLECT_ENABLE(FaustDsp)
};
DEFINE_MODEL_FACTORY(PhaserDspImpl, "Phaser", "Effect")

void InitFaustDspLibrary() {
    REGISTER_MODEL_FACTORY(DbMeterDspImpl);
    REGISTER_MODEL_FACTORY(FrenchBellDspImpl);
    REGISTER_MODEL_FACTORY(OscDspImpl);
    REGISTER_MODEL_FACTORY(FreeverbDspImpl);
    REGISTER_MODEL_FACTORY(FlangerDspImpl);
    REGISTER_MODEL_FACTORY(PhaserDspImpl);
}
