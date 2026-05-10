#include <augr/core/model_factory.h>
#include <augr/faust/faust_dsp.h>
#include <augr/faust/faust_dsp_ui.h>
#include <augr/rack/module/module.h>

#include <augr/exe/rack/exe_rack.h>

#include <augite/app/app.h>
#include <augite/view/rack_view.h>
#include <augite/widget/widget.h>
#include <augite/widget/widget_manufacturer.h>

#include "bubble_dsp.h"

using namespace augr;

class BubbleDspImpl : public BubbleDsp {
public:
    REFLECT_ENABLE(BubbleDsp)
};

class MyApp : public App {
public:
    MyApp() { view_ = new RackView(rack_); }

    void Draw() override {
        view_->Draw();
        App::Draw();
    }
    // Data members
    ExeRack rack_;
    RackView *view_;
};

int main(int, char **) {
    MyApp &app = *new MyApp();
    ExeRack &rack = app.rack_;
    rack.Create();
    rack.CreateDefaultDevices();

    ModelFactoryT<BubbleDspImpl>::Make(&rack);

    rack.Start();
    app.Run(augr::Window::RunParams("Augr Bubble"));
    rack.Stop();

    return 0;
}
