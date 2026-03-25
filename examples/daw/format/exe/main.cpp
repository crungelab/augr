#include <augr/rack/model_manufacturer.h>

#include <augr/rack/format/exe/exe_rack.h>
#include <augr/rack/module/faust_dsp.h>
#include <augr/rack/module/faust_dsp_ui.h>
#include <augr/rack/module/module.h>

#include <augite/app/app.h>

#include <augite/view/rack_view.h>
#include <augite/widget/widget_manufacturer.h>

using namespace augr;

class MyApp final : public App {
public:
    MyApp() {
        extern void InitFaustDspLibrary();
        InitFaustDspLibrary();
        // Create the rack view
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
