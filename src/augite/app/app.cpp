#include "imgui.h"

#include "imnodes.h"
#include "implot.h"

#include <augr/core/archiver_manufacturer.h>

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

    REGISTER_ARCHIVER_FACTORY(ModuleWidgetArchiver);
    REGISTER_ARCHIVER_FACTORY(RackViewArchiver);

    REGISTER_MODEL_WIDGET_FACTORY(SubrackWidget)
    REGISTER_MODEL_WIDGET_FACTORY(RackWidget)
    REGISTER_MODEL_WIDGET_FACTORY(DefaultModuleWidget)
    REGISTER_MODEL_WIDGET_FACTORY(ScopeWidget)
    REGISTER_MODEL_WIDGET_FACTORY(SpectralWidget)
    REGISTER_MODEL_WIDGET_FACTORY(ButtonWidget)
    REGISTER_MODEL_WIDGET_FACTORY(CheckButtonWidget)
    REGISTER_MODEL_WIDGET_FACTORY(ComboWidget)
    REGISTER_MODEL_WIDGET_FACTORY(VBoxWidget)
    REGISTER_MODEL_WIDGET_FACTORY(HBoxWidget)
    REGISTER_MODEL_WIDGET_FACTORY(NumEntryWidget)
    REGISTER_MODEL_WIDGET_FACTORY(HSliderWidget)
    REGISTER_MODEL_WIDGET_FACTORY(VSliderWidget)
    REGISTER_MODEL_WIDGET_FACTORY(HBarGraphWidget)
    REGISTER_MODEL_WIDGET_FACTORY(VBarGraphWidget)
    REGISTER_MODEL_WIDGET_FACTORY(KnobWidget)
};

bool App::DoCreate(CreateParams params) {
    clipboard_ = Clipboard();

    bool success = BaseApp::DoCreate(params);

    return success;
}

void App::CreateContext() {
    system_container_.Add(new ImGuiSystem());
    system_container_.Add(new ImPlotSystem());
    system_container_.Add(new ImNodesSystem());
    system_container_.Create();
}

void App::Draw() {
    frame_->Draw();
    BaseApp::Draw();
}

} // namespace augr