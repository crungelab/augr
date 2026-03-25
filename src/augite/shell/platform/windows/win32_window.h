#pragma once

#include <functional>
#include <memory>
#include <string>

#include <windows.h>

#include <augite/shell/glfw_window.h>

class Win32Window : public BaseWindow
{
public:
  Win32Window();
  virtual ~Win32Window();
};
