#pragma once

#include "../controller/controller.h"
#include "../view/view.h"

#include "../widget/frame.h"

namespace augr {

class Viewer : public Frame {
public:
    Viewer(const std::string &label = "") : Frame(label) {}
    virtual ~Viewer() = default;

    //void Draw() override;
    void End() override;

    // Accessors
    View &view() { return *view_; }
    const View &view() const { return *view_; }
    Controller &controller() { return *controller_; }
    const Controller &controller() const { return *controller_; }

    // Data members
protected:
    std::unique_ptr<View> view_;
    std::unique_ptr<Controller> controller_;
};

template <typename TView, typename TController, typename TBase = Viewer>
class ViewerT : public TBase {
public:
    template <typename... Args>
    ViewerT(const std::string &label = "", Args&&... args) : TBase(label, std::forward<Args>(args)...) {}
    // Accessors
    TView &view() { return *static_cast<TView *>(this->view_.get()); }
    TController &controller() {
        return *static_cast<TController *>(this->controller_.get());
    }
    //
    // Data members
};

} // namespace augr