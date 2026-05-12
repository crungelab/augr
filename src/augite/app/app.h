#pragma once

#include <augite/app/clipboard.h>

#include "base_app.h"
#include "../view/view.h"

namespace augr {

class App : public BaseApp {
public:
    App();

    void CreateContext() override;

    bool DoCreate(CreateParams params) override;
    void Draw() override;

    // Accessors
    static App &singleton() { return *singleton_; }
    Clipboard &clipboard() { return clipboard_; }

    // Data members
    static App *singleton_;
    View* view_;
    Clipboard clipboard_;
};

} // namespace augr