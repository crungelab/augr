#include <string>
#include <cassert>
#include <chrono>
#include <iostream>
#include <filesystem>

#include <augite/shell/shell.h>

#include "app.h"

#include "plug_editor_impl.h"
#include "plug_view_impl.h"

PlugViewImpl::PlugViewImpl(PannerEditor *editor, ViewRect *size)
    : PannerView(editor, size)
{
}

tresult PLUGIN_API PlugViewImpl::attached(void *parent, FIDString type)
{

    std::cout << "PlugViewImpl::attached" << std::endl
              << std::flush;

    parent_ = parent;

    app_ = new MyApp();

    running_ = true;
    renderThread_ = std::thread([this]()
                                { this->Run(); });
    return PlugView::attached(parent, type);
}

void PlugViewImpl::Run()
{
    const augr::Window::RunParams runParams("Augr Panner", augr::Window::Point(0, 0),
                                        augr::Window::Size(rect.getWidth(), rect.getHeight()),
                                        parent_);
    app_->Run(runParams);
}

tresult PLUGIN_API PlugViewImpl::removed()
{
    //app_->Destroy();
    app_->RequestClose();
    running_ = false;
    if (renderThread_.joinable())
        renderThread_.join();
    return PlugView::removed();
}

tresult PLUGIN_API PlugViewImpl::onSize(ViewRect *newSize)
{
    if (app_ == nullptr)
        return PlugView::onSize(newSize);

    int w = newSize->getWidth();
    int h = newSize->getHeight();
    std::cout << "RackViewImpl::onSize " << w << "x" << h << std::endl
              << std::flush;
    app_->RequestResize(w, h);
    return PlugView::onSize(newSize);
}
