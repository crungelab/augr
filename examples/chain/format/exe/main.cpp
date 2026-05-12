#include <augr/core/model_manufacturer.h>

#include <augr/faust/faust_dsp.h>
#include <augr/faust/faust_dsp_ui.h>

#include <augr/exe/rack/exe_rack.h>

#include <augite/app/rack_app.h>

#include "chain_dsp.h"

using namespace augr;

class ChainDspImpl : public ChainDsp {
    REFLECT_ENABLE(FaustDsp)
};

int main(int, char **) {
    REGISTER_MODEL_FACTORY(ExeRack);

    RackApp app;
    Rack &rack = app.rack();

    ModelFactoryT<ChainDspImpl>::Make(&rack);

    app.Run(augr::Window::RunParams("Augr Chain"));

    return 0;
}
