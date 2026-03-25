#include <augr/core/rack/model_manufacturer.h>

#include <augr/rtaudio/rack/rtaudio_rack.h>
#include <augr/core/rack/module/faust_dsp.h>
#include <augr/core/rack/module/faust_dsp_ui.h>
#include <augr/core/rack/module/module.h>

#include <augite/app/app.h>

#include <augite/view/rack_view.h>
#include <augite/widget/widget_manufacturer.h>

#include "frenchbell_dsp.h"
#include "osc_dsp.h"
#include "freeverb_dsp.h"
#include "flanger_dsp.h"
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

class MyApp final : public App {
public:
    MyApp() {
        /*
        OscDsp &m = ModelFactoryT<OscDspImpl>::Make(rack_);
        rack_.AddChild(m);

        FreeVerbDsp &m2 = ModelFactoryT<FreeVerbDspImpl>::Make(rack_);
        rack_.AddChild(m2);

        FlangerDsp &m3 = ModelFactoryT<FlangerDspImpl>::Make(rack_);
        rack_.AddChild(m3);

        PhaserDsp &m4 = ModelFactoryT<PhaserDspImpl>::Make(rack_);
        rack_.AddChild(m4);
        */

        REGISTER_MODEL_FACTORY(FrenchBellDspImpl);
        REGISTER_MODEL_FACTORY(OscDspImpl);
        REGISTER_MODEL_FACTORY(FreeVerbDspImpl);
        REGISTER_MODEL_FACTORY(FlangerDspImpl);
        REGISTER_MODEL_FACTORY(PhaserDspImpl);

        view_ = new RackView(rack_);
    }

    void Draw() override {
        App::Draw();
        view_->Draw();
    }
    // Data members
    ExeRack rack_;
    RackView *view_;
};

int main(int, char **) {
    MyApp &app = *new MyApp();
    ExeRack &rack = app.rack_;
    rack.Create();
    rack.Start();
    app.Run();
    rack.Stop();
}
