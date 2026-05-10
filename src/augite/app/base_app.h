#pragma once

#include <augite/shell/shell.h>

#include <augite/app/clipboard.h>

namespace augr {

class Rack;

class BaseApp : public Window {
public:
    BaseApp();

    bool DoCreate(CreateParams params) override;
    bool PostCreate(CreateParams params) override;
    void Destroy() override;

    void Render() override;
};

} // namespace augr