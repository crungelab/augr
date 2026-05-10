#pragma once

#include <augite/app/clipboard.h>

#include "base_app.h"

namespace augr {

class Rack;

class App : public BaseApp {
public:
    App();

    void CreateContext() override;

    bool DoCreate(CreateParams params) override;

    // Accessors
    static App &singleton() { return *singleton_; }
    Clipboard &clipboard() { return clipboard_; }
    Rack& rack() { return *rack_; }

    // Data members
    static App *singleton_;
    Clipboard clipboard_;
    Rack* rack_ = nullptr;
};

} // namespace augr