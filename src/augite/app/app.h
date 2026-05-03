#pragma once

#include <augite/shell/shell.h>

namespace augr {

class App : public Window {
public:
    App();
    // virtual ~App();

    void CreateContext() override;

    bool DoCreate(CreateParams params) override;
    bool PostCreate(CreateParams params) override;
    void Destroy() override;

    void Render() override;

    // Accessors
    static App &singleton() { return *singleton_; }

    // Data members
    static App *singleton_;
};

} // namespace augr