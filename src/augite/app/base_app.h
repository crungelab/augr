#pragma once

#include <augite/shell/shell.h>

namespace augr {

class Rack;

class BaseApp : public Window {
public:
    BaseApp();

    bool DoCreate(CreateParams params) override;
    void OnDestroy() override;

    void Render() override;
};

} // namespace augr