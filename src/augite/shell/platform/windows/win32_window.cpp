#include <GLFW/glfw3.h>

#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>

#include "win32_window.h"

#include "resource.h"

Win32Window::Win32Window() {
}

Win32Window::~Win32Window() {
    Destroy();
}
