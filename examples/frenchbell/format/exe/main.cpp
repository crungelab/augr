#include <augr/model_manufacturer.h>

#include <augr/faust/faust_dsp.h>
#include <augr/faust/faust_dsp_ui.h>

#include <augr/exe/rack/exe_rack.h>

#include <augite/app/rack_app.h>

#include "frenchbell_dsp.h"

using namespace augr;

class FrenchBellDspImpl : public FrenchBellDsp {
public:
    REFLECT_ENABLE(FrenchBellDsp)
};

int main(int, char **) {
    REGISTER_MODEL_FACTORY(ExeRack);

    RackApp app;
    const Rack::Ptr rack = app.document().model_;

    ModelFactoryT<FrenchBellDspImpl>::Make(rack);

    app.Run(augr::Window::RunParams("Augr French Bell"));

    return 0;
}
