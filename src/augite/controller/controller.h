#pragma once

#include <augr/core/document.h>

#include "../view/view.h"

namespace augr {

class Frame;

class Controller {
public:
    Controller(Document &doc, View &view, Frame &frame)
        : doc_(&doc), view_(&view), frame_(&frame) {}
    virtual ~Controller() = default;

    virtual void Control() = 0;

    Frame &frame() { return *frame_; }
    const Frame &frame() const { return *frame_; }

    // Data members
    Document *doc_;
    View *view_;
    Frame *frame_;
};

template <typename TDoc, typename TView> class ControllerT : public Controller {
public:
    ControllerT(TDoc &doc, TView &view, Frame &frame)
        : Controller(doc, view, frame) {}

    TDoc &doc() { return *static_cast<TDoc *>(doc_); }
    const TDoc &doc() const { return *static_cast<const TDoc *>(doc_); }

    TView &view() { return *static_cast<TView *>(view_); }
    const TView &view() const { return *static_cast<const TView *>(view_); }
};

} // namespace augr