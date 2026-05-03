#pragma once

#include <augite/shell/shell.h>

#include <augite/app/clipboard.h>

namespace augr {

class App : public Window {
public:
    App();

    void CreateContext() override;

    bool DoCreate(CreateParams params) override;
    bool PostCreate(CreateParams params) override;
    void Destroy() override;

    void Render() override;

    // Accessors
    static App &singleton() { return *singleton_; }
    Clipboard &clipboard() { return clipboard_; }

    // Data members
    static App *singleton_;
    Clipboard clipboard_;
};

} // namespace augr