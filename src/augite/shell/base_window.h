#pragma once

#include <functional>
#include <memory>
#include <string>

// #include <mutex>
// #include <queue>

#include "imgui.h"

#include "../system/system.h"

struct SDL_Window;
struct SDL_GPUDevice;
union SDL_Event;

namespace augr {

class BaseWindow {
public:
    struct Point {
        unsigned int x;
        unsigned int y;
        Point() {}
        Point(unsigned int x, unsigned int y) : x(x), y(y) {}
        Point(const Point &p1) {
            x = p1.x;
            y = p1.y;
        }
    };

    struct Size {
        unsigned int width;
        unsigned int height;
        Size() {}
        Size(unsigned int width, unsigned int height)
            : width(width), height(height) {}
        Size(const Size &s1) {
            width = s1.width;
            height = s1.height;
        }
    };
    //
    // Create
    //
    struct CreateParams {
        CreateParams(std::string _title = "No Name",
                     Point _origin = Point(0, 0), Size _size = Size(800, 600),
                     void *_nativeParent = nullptr) {
            title = _title;
            origin = _origin;
            size = _size;
            nativeParent = _nativeParent;
        }
        std::string title;
        Point origin;
        Size size;
        void *nativeParent = nullptr;
    };

    struct RunParams : CreateParams {
        /*
        RunParams(std::string _title = "No Name", Point _origin = Point(0, 0),
        Size _size = Size(800, 600), void *_nativeParent = nullptr)
        {
          title = _title;
          origin = _origin;
          size = _size;
          nativeParent = _nativeParent;
        }
        */
        using CreateParams::CreateParams;
    };

    BaseWindow();
    virtual ~BaseWindow();

    bool Create(CreateParams params = CreateParams()) {
        if (!DoCreate(params)) {
            return false;
        }
        if (!PostCreate(params)) {
            return false;
        }
        return true;
    }

    virtual bool DoCreate(CreateParams params);
    virtual bool PostCreate(CreateParams params) { return true; }

    bool CreateAndShow(CreateParams params = CreateParams()) {
        if (!Create(params)) {
            return false;
        }
        return Show();
    }
    virtual bool Show() { return true; }
    virtual bool Hide() { return true; }

    virtual void NativeAttachTo(void *nativeParent) {}
    /*
     * Context
     */
    virtual void CreateContext() {}
    virtual void DestroyContext();

    //
    // Destroy
    //
    virtual void Destroy();

    //
    // Events
    //
    virtual bool Dispatch(const SDL_Event &event);

    //
    // Run
    //

    bool Run(RunParams params = RunParams()) {
        if (!DoRun(params)) {
            return false;
        }
        if (!PostRun(params)) {
            return false;
        }
        return true;
    }

    virtual bool DoRun(RunParams params);
    virtual bool PostRun(RunParams params);

    //
    virtual void Render() {}
    virtual void Draw() {}
    // Events
    virtual void OnWindowRefresh() { Render(); }
    virtual void OnWindowSize(int width, int height) {}
    virtual void OnWindowPos(int xpos, int ypos) {}
    virtual void OnCursorPos(double x, double y) {}
    virtual void OnMouseButton(int button, int action, int mods) {}
    virtual void OnWindowFocus(int focused) {}

    // If true, closing this window will quit the application.
    void SetQuitOnClose(bool quit_on_close);

    // Called from other threads (EditorView::onSize)
    void RequestResize(int w, int h); // thread-safe
    void RequestClose();              // thread-safe

    // Data members
    bool is_embedded_ = false;
    SystemContainer system_container_;
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
    //
protected:
    bool quit_on_close_ = false;
    //
public:
    SDL_Window *window_ = nullptr;
    SDL_GPUDevice *gpu_device_ = nullptr;

private:
    struct Cmd {
        enum Kind { Resize } kind;
        int w, h;
    };
    // std::mutex mtx_;
    // std::queue<Cmd> q_;              // simple thread-safe queue
    void ApplyResize(int w, int h); // runs on window thread
    void ApplyClose();
};

} // namespace augr