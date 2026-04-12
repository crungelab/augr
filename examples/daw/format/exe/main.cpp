#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>

#include <augr/core/model_manufacturer.h>

#include <augr/exe/rack/exe_rack.h>
#include <augr/faust/faust_dsp.h>
#include <augr/faust/faust_dsp_ui.h>
#include <augr/core/rack/module/module.h>

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

int main(int, char**) {
    auto logger = spdlog::stdout_color_mt("console");
    spdlog::set_default_logger(logger);

    spdlog::set_level(spdlog::level::debug);
    spdlog::flush_on(spdlog::level::debug);

    spdlog::debug("This message should be displayed.");

    MyApp &app = *new MyApp();
    ExeRack &rack = app.rack_;
    rack.Create();
    rack.Start();
    app.Run();
    rack.Stop();
}
