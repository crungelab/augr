
/*#include <GLFW/glfw3.h>
#define GLFW_EXPOSE_NATIVE_X11
#include <GLFW/glfw3native.h>*/

// SDL3 + native X11 access
#include <SDL3/SDL.h>
#include <SDL3/SDL_video.h>
#include <SDL3/SDL_properties.h>

// X11
#include <X11/Xlib.h>

#include "x11_window.h"

namespace augr
{

X11Window::X11Window()
{
}

X11Window::~X11Window()
{
    Destroy();
}

void X11Window::NativeAttachTo(void* nativeParent)
{
    if (!window_ || !nativeParent)
        return;

    // Ensure we’re on X11 (Wayland won’t have these properties)
    const char* vdriver = SDL_GetCurrentVideoDriver();
    if (!vdriver || strcmp(vdriver, "x11") != 0) {
        SDL_Log("NativeAttachTo: not on X11 (current video driver: %s)", vdriver ? vdriver : "(none)");
        return;
    }  // SDL_GetCurrentVideoDriver docs. :contentReference[oaicite:1]{index=1}

    SDL_PropertiesID props = SDL_GetWindowProperties(window_);
    if (!props) {
        SDL_Log("NativeAttachTo: failed to get window properties: %s", SDL_GetError());
        return;
    }

    // Fetch X11 Display* and Window ID from SDL window properties
    // Property names from SDL3 docs:
    //  - SDL_PROP_WINDOW_X11_DISPLAY_POINTER
    //  - SDL_PROP_WINDOW_X11_WINDOW_NUMBER
    // (screen number exists too, but not needed here). :contentReference[oaicite:2]{index=2}
    Display* dpy = static_cast<Display*>(
        SDL_GetPointerProperty(props, SDL_PROP_WINDOW_X11_DISPLAY_POINTER, nullptr)
    );
    ::Window child = static_cast<::Window>(
        SDL_GetNumberProperty(props, SDL_PROP_WINDOW_X11_WINDOW_NUMBER, 0)
    );

    if (!dpy || !child) {
        SDL_Log("NativeAttachTo: missing X11 handles (dpy=%p, child=%lu)", (void*)dpy, (unsigned long)child);
        return;
    }

    ::Window new_parent = (::Window)(uintptr_t)nativeParent;

    // Optional: tweak override_redirect / XEMBED info if your host requires it.
    // (Left commented exactly like your original.)

    // Unmap to avoid flicker and let the server update hierarchy cleanly
    XUnmapWindow(dpy, child);

    // Do the reparent
    XReparentWindow(dpy, child, new_parent, 0, 0);

    // Remap and flush
    XMapWindow(dpy, child);
    XFlush(dpy);
}

} // namespace augr