#pragma once

#include <augite/app/clipboard.h>

#include "base_app.h"
#include "../frame/frame.h"

namespace augr {

class App : public BaseApp {
public:
    App();

    void CreateContext() override;

    bool DoCreate(CreateParams params) override;
    void Draw() override;

    // Accessors
    static App &singleton() { return *singleton_; }
    Frame &frame() { return *frame_; }
    Clipboard &clipboard() { return clipboard_; }

    // Data members
    static App *singleton_;
    Frame* frame_;
    Clipboard clipboard_;
};

} // namespace augr