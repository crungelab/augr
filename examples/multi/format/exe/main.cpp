#include <augr/model_manufacturer.h>

#include <augr/faust/faust_dsp.h>
#include <augr/faust/faust_dsp_ui.h>

#include <augr/exe/rack/exe_rack.h>

#include <augite/app/rack_app.h>

#include "freeverb_dsp.h"
#include "osc_dsp.h"

using namespace augr;

class OscDspImpl : public OscDsp {
    REFLECT_ENABLE(FaustDsp)
};

class FreeVerbDspImpl : public FreeVerbDsp {
    REFLECT_ENABLE(FaustDsp)
};

int main(int, char **) {
    REGISTER_MODEL_FACTORY(ExeRack);

    RackApp app;
    const Rack::Ptr rack = app.document().model_;

    ModelFactoryT<OscDspImpl>::Make(rack);
    ModelFactoryT<FreeVerbDspImpl>::Make(rack);

    app.Run(augr::Window::RunParams("Augr Multi"));

    return 0;
}
