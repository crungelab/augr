#pragma once

#include "augr/core/document.h"
#include "../frame/frame.h"
#include "base_app.h"

namespace augr {

class App : public BaseApp {
public:
    App();

    void CreateContext() override;

    bool DoCreate(CreateParams params) override;
    void Draw() override;

    // Accessors
    static App &singleton() { return *singleton_; }
    Frame &root_frame() { return *root_frame_; }
    Frame *active_frame() { return active_frame_; }
    void set_active_frame(Frame *f) { active_frame_ = f; }

    // Data members
    static App *singleton_;
    std::unique_ptr<Document> doc_;
    std::unique_ptr<Frame> root_frame_;
    Frame *active_frame_ = nullptr; // last frame to have focus};
};

} // namespace augr