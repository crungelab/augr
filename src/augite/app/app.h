#pragma once

#include <memory>
#include <vector>

#include "../widget/frame.h"
#include "../inspector/inspector_dock.h"
#include "augr/core/document.h"

#include "base_app.h"

namespace augr {

class Widget;

class App : public BaseApp {
public:
    App();

    void CreateContext() override;

    void Draw() override;

    std::vector<std::function<void()>> deferred_actions_;

    void QueueAction(std::function<void()> action) {
        deferred_actions_.push_back(std::move(action));
    }

    void RunDeferred() {
        for (auto &action : deferred_actions_)
            action();
        deferred_actions_.clear();
    }
    
    // Schedule a widget (and its subtree) for destruction at the next
    // safe point. Widget::Destroy() forwards here via GetDestroyQueue().
    // Safe to call from inside event handlers and draw code.
    void ScheduleDestroy(std::unique_ptr<Widget> widget);

    // Drain the pending-destroy queue. Call once per frame at a safe
    // boundary — typically between event dispatch and the next Draw(),
    // or at the end of the frame after Draw() returns.
    void ProcessPendingDestroy();

    void Inspect(Model *model);

    // Accessors
    static App &singleton() { return *singleton_; }
    Frame &root_frame() { return *root_frame_; }
    Frame *active_frame() { return active_frame_; }
    void set_active_frame(Frame *f) { active_frame_ = f; }

    InspectorDock &inspector_dock() { return *inspector_dock_; }
    void set_inspector_dock(std::unique_ptr<InspectorDock> insp) {
        inspector_dock_ = std::move(insp);
    }

    // Data members
    static App *singleton_;
    std::unique_ptr<Document> doc_;
    std::unique_ptr<Frame> root_frame_;
    Frame *active_frame_ = nullptr; // last frame to have focus
    std::vector<std::unique_ptr<Widget>> pending_destroy_;
    std::unique_ptr<InspectorDock> inspector_dock_;
    Model *inspected_model_ = nullptr;
};

} // namespace augr