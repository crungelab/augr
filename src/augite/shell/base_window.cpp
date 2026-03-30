#include <SDL3/SDL.h>

#include "imgui.h"
#include <backends/imgui_impl_sdl3.h>

#include "base_window.h"
#include "../event/window_event.h"

namespace augr
{

void BaseWindow::DestroyContext()
{
    system_container_.Destroy();
}

BaseWindow::BaseWindow()
{
}

BaseWindow::~BaseWindow()
{
    Destroy();
}

bool BaseWindow::DoCreate(CreateParams params)
{
    CreateContext();

    if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMEPAD))
    {
        printf("Error: SDL_Init(): %s\n", SDL_GetError());
        return false;
    }

    is_embedded_ = (params.nativeParent != nullptr);

    uint32_t sdl_flags = SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE;

    if (is_embedded_)
    {
        sdl_flags |= SDL_WINDOW_BORDERLESS;
    }

    window_ = SDL_CreateWindow(params.title.c_str(),
                               params.size.width,
                               params.size.height,
                               sdl_flags);

    if (window_ == NULL)
        return false;

    if (params.nativeParent)
    {
        NativeAttachTo(params.nativeParent);
    }

    return true;
}

void BaseWindow::SetQuitOnClose(bool quit_on_close)
{
    quit_on_close_ = quit_on_close;
}

bool BaseWindow::Dispatch(const SDL_Event &event) {
    if (auto* raw = TryDecode<ResizeEvent>(event)) {
        auto ev = Adopt(raw);
        ApplyResize(ev->width, ev->height);
    } else if (auto* raw = TryDecode<CloseEvent>(event)) {
        auto ev = Adopt(raw);
        //ApplyClose();
        return true;
    }

    ImGui_ImplSDL3_ProcessEvent(&event);

    if (event.type == SDL_EVENT_QUIT)
        return true;
    if (event.type == SDL_EVENT_WINDOW_CLOSE_REQUESTED && event.window.windowID == SDL_GetWindowID(window_))
        return true;

    return false;
}

bool BaseWindow::DoRun(RunParams params)
{
    bool success = CreateAndShow(params);

    assert(success);
    if (!success)
    {
        return false;
    }

    // Main loop
    const int FPS = 60;
    const int frameDelay = 1000 / FPS;
    Uint32 frameStart;
    int frameTime;
    bool done = false;

    while (!done)
    {
        frameStart = SDL_GetTicks();
        //Render();
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            done = Dispatch(event);
        }

        Render();

        frameTime = SDL_GetTicks() - frameStart;
        if (frameDelay > frameTime) {
            SDL_Delay(frameDelay - frameTime);
        }
    }

    return true;
}

bool BaseWindow::PostRun(RunParams params)
{
    Destroy();
    return true;
}

void BaseWindow::Destroy()
{
    DestroyContext();

    if (window_)
    {
        SDL_DestroyWindow(window_);
        window_ = nullptr;
    }
    SDL_Quit();
}

void BaseWindow::RequestResize(int w, int h)
{
    Post<augr::ResizeEvent>(w, h);
}

void BaseWindow::RequestClose()
{
    Post<augr::CloseEvent>();
}

void BaseWindow::ApplyResize(int w, int h)
{
    // Single place that touches SDL size; we’re on the window thread
    SDL_SetWindowSize(window_, w, h);
}

void BaseWindow::ApplyClose()
{
    // Handle close request
}

} // namespace augr