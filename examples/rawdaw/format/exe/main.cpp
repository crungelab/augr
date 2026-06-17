#include <augr/model_manufacturer.h>

#include <augr/exe/rack/exe_rack.h>
#include <augr/faust/faust_dsp.h>
#include <augr/faust/faust_dsp_ui.h>
#include <augr/rack/module/module.h>

#include <augite/app/app.h>

#include <augite/widget/widget_manufacturer.h>

#include "flanger_dsp.h"
#include "freeverb_dsp.h"
#include "frenchbell_dsp.h"
#include "osc_dsp.h"
#include "phaser_dsp.h"

using namespace augr;

class FrenchBellDspImpl : public FrenchBellDsp {
public:
    REFLECT_ENABLE(FrenchBellDsp)
};
DEFINE_MODEL_FACTORY(FrenchBellDspImpl, "French Bell", "Instrument")

class OscDspImpl final : public OscDsp {
    REFLECT_ENABLE(FaustDsp)
};
DEFINE_MODEL_FACTORY(OscDspImpl, "Oscillator", "Generator")

class FreeVerbDspImpl final : public FreeVerbDsp {
    REFLECT_ENABLE(FaustDsp)
};
DEFINE_MODEL_FACTORY(FreeVerbDspImpl, "Freeverb", "Reverb")

class FlangerDspImpl final : public FlangerDsp {
    REFLECT_ENABLE(FaustDsp)
};
DEFINE_MODEL_FACTORY(FlangerDspImpl, "Flanger", "Effect")

class PhaserDspImpl final : public PhaserDsp {
    REFLECT_ENABLE(FaustDsp)
};
DEFINE_MODEL_FACTORY(PhaserDspImpl, "Phaser", "Effect")

int main(int, char **) {
    REGISTER_MODEL_FACTORY(ExeRack);

    REGISTER_MODEL_FACTORY(FrenchBellDspImpl);
    REGISTER_MODEL_FACTORY(OscDspImpl);
    REGISTER_MODEL_FACTORY(FreeVerbDspImpl);
    REGISTER_MODEL_FACTORY(FlangerDspImpl);
    REGISTER_MODEL_FACTORY(PhaserDspImpl);

    App &app = *new App();

    app.Run(augr::Window::RunParams("Augr Raw DAW"));

    return 0;
}
