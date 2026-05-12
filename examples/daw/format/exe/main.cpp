#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>

#include <augr/core/model_manufacturer.h>

#include <augr/exe/rack/exe_rack.h>

#include <augite/app/rack_app.h>

using namespace augr;

int main(int, char **) {
    REGISTER_MODEL_FACTORY(ExeRack);
    
    extern void InitAugrRackLibrary();
    InitAugrRackLibrary();
    extern void InitAugrVoltLibrary();
    InitAugrVoltLibrary();
    extern void InitAugrFmLibrary();
    InitAugrFmLibrary();
    extern void InitFaustDspLibrary();
    InitFaustDspLibrary();

    auto logger = spdlog::stdout_color_mt("console");
    spdlog::set_default_logger(logger);

    spdlog::set_level(spdlog::level::debug);
    spdlog::flush_on(spdlog::level::debug);

    spdlog::debug("This message should be displayed.");

    RackApp app;
    app.Run(augr::Window::RunParams("Augr DAW"));
}
