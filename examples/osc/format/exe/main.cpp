#include <augr/core/model_manufacturer.h>

#include <augr/faust/faust_dsp.h>
#include <augr/faust/faust_dsp_ui.h>

#include <augr/exe/rack/exe_rack.h>

#include <augite/app/rack_app.h>

#include "osc_dsp.h"

using namespace augr;

class OscDspImpl : public OscDsp {
    REFLECT_ENABLE(FaustDsp)
};

int main(int, char **) {
    REGISTER_MODEL_FACTORY(ExeRack);

    RackApp app;
    Rack &rack = app.rack();

    ModelFactoryT<OscDspImpl>::Make(&rack);

    app.Run(augr::Window::RunParams("Augr Osc"));

    return 0;
}
