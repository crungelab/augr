#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>

#include <augr/core/rack/model_manufacturer.h>

#include <augr/rtaudio/rack/rtaudio_rack.h>
#include <augr/core/rack/module/faust_dsp.h>
#include <augr/core/rack/module/faust_dsp_ui.h>
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
    RtAudioRack rack_;
    RackView *view_;
};

int main(int, char**) {
    auto logger = spdlog::stdout_color_mt("console");
    spdlog::set_default_logger(logger);

    spdlog::set_level(spdlog::level::debug);
    spdlog::flush_on(spdlog::level::debug);

    spdlog::debug("This message should be displayed.");

    MyApp &app = *new MyApp();
    RtAudioRack &rack = app.rack_;
    rack.Create();
    rack.Start();
    app.Run();
    rack.Stop();
}
