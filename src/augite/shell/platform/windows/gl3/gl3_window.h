#pragma once

#include <augite/shell/platform/windows/win32_window.h>

class Gl3Window : public Win32Window {
public:
  virtual void Destroy();
  virtual bool DoCreate(CreateParams params) override;
  virtual bool PostCreate(CreateParams params) override;
  virtual void Render() override;
  void Draw() override;
  //Data members
  const char* glsl_version_ = nullptr;
};