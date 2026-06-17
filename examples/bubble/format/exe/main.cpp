#include <augr/model_manufacturer.h>

#include <augr/faust/faust_dsp.h>
#include <augr/faust/faust_dsp_ui.h>

#include <augr/exe/rack/exe_rack.h>

#include <augite/app/rack_app.h>

#include "bubble_dsp.h"

using namespace augr;

class BubbleDspImpl : public BubbleDsp {
public:
    REFLECT_ENABLE(BubbleDsp)
};

int main(int, char **) {
    REGISTER_MODEL_FACTORY(ExeRack);

    RackApp app;
    const Rack::Ptr rack = app.document().model_;

    ModelFactoryT<BubbleDspImpl>::Make(rack);

    app.Run(augr::Window::RunParams("Augr Bubble"));

    return 0;
}
