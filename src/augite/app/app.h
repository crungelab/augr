#pragma once

#include <memory>
#include <vector>

#include "augr/core/document.h"
#include "../frame/frame.h"
#include "../inspector/inspector.h"

#include "base_app.h"

namespace augr {

class Widget;

class App : public BaseApp {
public:
    App();

    void CreateContext() override;

    bool DoCreate(CreateParams params) override;
    void Draw() override;

    // Schedule a widget (and its subtree) for destruction at the next
    // safe point. Widget::Destroy() forwards here via GetDestroyQueue().
    // Safe to call from inside event handlers and draw code.
    void ScheduleDestroy(std::unique_ptr<Widget> widget);

    // Drain the pending-destroy queue. Call once per frame at a safe
    // boundary — typically between event dispatch and the next Draw(),
    // or at the end of the frame after Draw() returns.
    void ProcessPendingDestroy();

    // Accessors
    static App &singleton() { return *singleton_; }
    Frame &root_frame() { return *root_frame_; }
    Frame *active_frame() { return active_frame_; }
    void set_active_frame(Frame *f) { active_frame_ = f; }

    Inspector &inspector() { return *inspector_; }
    void set_inspector(std::unique_ptr<Inspector> insp) { inspector_ = std::move(insp); }

    // Data members
    static App *singleton_;
    std::unique_ptr<Document> doc_;
    std::unique_ptr<Frame> root_frame_;
    Frame *active_frame_ = nullptr; // last frame to have focus
    std::vector<std::unique_ptr<Widget>> pending_destroy_;
    std::unique_ptr<Inspector> inspector_;
};

} // namespace augr