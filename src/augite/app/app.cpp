#include "imgui.h"

#include "imnodes.h"
#include "implot.h"

#include <augr/core/archiver_manufacturer.h>

#include "../widget/widget_manufacturer.h"

#include "../viewer/viewer_manufacturer.h"
#include "../viewer/viewer_factory.h"

#include "../inspector/inspector_builder.h"
#include "../inspector/inspector_manufacturer.h"

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
    REGISTER_ARCHIVER_FACTORY(SubrackWidgetArchiver);

    REGISTER_ARCHIVER_FACTORY(SubrackViewArchiver)

    REGISTER_ARCHIVER_FACTORY(ViewerArchiver);
    REGISTER_ARCHIVER_FACTORY(ModuleViewerArchiver);
    REGISTER_ARCHIVER_FACTORY(SubrackViewerArchiver);
    REGISTER_ARCHIVER_FACTORY(RackViewerArchiver);


    REGISTER_MODEL_WIDGET_FACTORY(SubrackWidget)
    REGISTER_MODEL_WIDGET_FACTORY(RackWidget)
    REGISTER_MODEL_WIDGET_FACTORY(ModuleWidget)
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

    REGISTER_INSPECTOR_FACTORY(ModuleInspector)
    REGISTER_INSPECTOR_FACTORY(VoicebankInspector)

    REGISTER_VIEWER_FACTORY(ModuleViewer)
    REGISTER_VIEWER_FACTORY(SubrackViewer)
    REGISTER_VIEWER_FACTORY(DexieVoiceViewer)

    REGISTER_VIEWER_FACTORY(ScopeViewer)
    REGISTER_VIEWER_FACTORY(SpectralViewer)
};

void App::CreateContext() {
    system_container_.Add(new ImGuiSystem());
    system_container_.Add(new ImPlotSystem());
    system_container_.Add(new ImNodesSystem());
    system_container_.Create();
}

void App::ScheduleDestroy(std::unique_ptr<Widget> widget) {
    if (!widget)
        return;
    pending_destroy_.push_back(std::move(widget));
}

void App::ProcessPendingDestroy() {
    // Swap-and-drain. Widgets whose destructors schedule further
    // destruction land in the next frame's queue, not this one,
    // which keeps iteration safe and bounds work per frame.
    std::vector<std::unique_ptr<Widget>> to_delete;
    to_delete.swap(pending_destroy_);
    // unique_ptrs destruct here in order, cascading through subtrees.
}

// Bridge between widget.h's DestroyQueue interface and App's singleton,
// so widget.h doesn't need to include app.h.
namespace {
class AppDestroyQueue : public DestroyQueue {
public:
    void ScheduleDestroy(std::unique_ptr<Widget> widget) override {
        App::singleton().ScheduleDestroy(std::move(widget));
    }
};
AppDestroyQueue g_destroy_queue;
} // namespace

DestroyQueue &GetDestroyQueue() { return g_destroy_queue; }

void App::Draw() {
    root_frame_->Draw();
    if (inspector_dock_)
        inspector_dock_->Draw();
    BaseApp::Draw();

    ProcessPendingDestroy();
    RunDeferred();
}

void App::Inspect(Model *model) {
    if (model == inspected_model_) {
        return; // already inspecting this model
    }
    inspected_model_ = model;

    if (model == nullptr) {
        set_inspector_dock(nullptr);
        return;
    }

    auto root = InspectorBuilder().Build(*model);
    set_inspector_dock(std::make_unique<InspectorDock>(std::move(root)));
}

} // namespace augr