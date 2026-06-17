#pragma once

#include <functional>
#include <memory>
#include <string>

#include <augite/shell/base_window.h>

namespace augr
{

  class X11Window : public BaseWindow
  {
  public:
    X11Window();
    virtual ~X11Window();
    void NativeAttachTo(void *nativeParent) override;
  };

} // namespace augr