#pragma once

#include <augite/shell/platform/windows/win32_window.h>

class Dx12Window : public Win32Window {
  public:
    virtual bool PostCreate(CreateParams params) override;
    virtual bool PostRun(RunParams params) override;
    virtual void Render() override;
    void Draw() override;
    virtual void Destroy() override;
    //
    virtual void OnWindowSize(int width, int height) override;
};