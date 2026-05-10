#include "imgui.h"
#include <backends/imgui_impl_sdl3.h>
#include <backends/imgui_impl_sdlgpu3.h>

#include "imnodes.h"
#include "implot.h"

#include "../widget/widget_manufacturer.h"

#include "../system/imgui_system.h"
#include "../system/imnodes_system.h"
#include "../system/implot_system.h"

#include "../event/event.h"

#include "app.h"

namespace augr {

App *App::singleton_;

App::App() {
    singleton_ = this;
};

bool App::DoCreate(CreateParams params) {

    bool success = BaseApp::DoCreate(params);

    REGISTER_WIDGET_FACTORY(RackWidget)
    REGISTER_WIDGET_FACTORY(DefaultModuleWidget)
    REGISTER_WIDGET_FACTORY(ScopeWidget)
    REGISTER_WIDGET_FACTORY(SpectralWidget)
    REGISTER_WIDGET_FACTORY(ButtonWidget)
    REGISTER_WIDGET_FACTORY(CheckButtonWidget)
    REGISTER_WIDGET_FACTORY(ComboWidget)
    REGISTER_WIDGET_FACTORY(VBoxWidget)
    REGISTER_WIDGET_FACTORY(HBoxWidget)
    REGISTER_WIDGET_FACTORY(NumEntryWidget)
    REGISTER_WIDGET_FACTORY(HSliderWidget)
    REGISTER_WIDGET_FACTORY(VSliderWidget)
    REGISTER_WIDGET_FACTORY(HBarGraphWidget)
    REGISTER_WIDGET_FACTORY(VBarGraphWidget)
    REGISTER_WIDGET_FACTORY(KnobWidget)

    clipboard_ = Clipboard();
    
    return success;
}

void App::CreateContext() {
    system_container_.Add(new ImGuiSystem());
    system_container_.Add(new ImPlotSystem());
    system_container_.Add(new ImNodesSystem());
    system_container_.Create();
}

} // namespace augr