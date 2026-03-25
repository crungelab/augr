#include <augr/core/rack/format/exe/exe_rack.h>
#include <augr/core/rack/module/faust_dsp.h>
#include <augr/core/rack/module/faust_dsp_ui.h>
#include <augr/core/rack/module/module.h>

#include <augite/app/app.h>

#include <augite/view/rack_view.h>
#include <augite/widget/widget.h>
#include <augite/widget/widget_manufacturer.h>

#include "frenchbell_dsp.h"

using namespace augr;

class FrenchBellDspImpl : public FrenchBellDsp {
public:
    REFLECT_ENABLE(FrenchBellDsp)
};

class MyApp : public App {
public:
    MyApp() {
        FrenchBellDsp &m = ModelFactoryT<FrenchBellDspImpl>::Make(rack_);
        rack_.AddChild(m);
        view_ = new RackView(rack_);
    }

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
    rack.Start();
    app.Run();
    rack.Stop();

    return 0;
}
