#pragma once

#include <augr/core/document.h>

#include "../view/view.h"

namespace augr {

class Controller {
public:
    Controller(Document& doc, View& view) : doc_(&doc), view_(&view) {}
    virtual void Control() = 0;
    // Data members
    Document* doc_;
    View* view_;
};

template <typename TDoc, typename TView> class ControllerT : public Controller {
public:
    ControllerT(TDoc& doc, TView& view) : Controller(doc, view) {}
    // Accessors
    TDoc &doc() { return *static_cast<TDoc *>(doc_); }
    const TDoc &doc() const { return *static_cast<const TDoc *>(doc_); }

    TView &view() { return *static_cast<TView *>(view_); }
    const TView &view() const { return *static_cast<const TView *>(view_); }
    //
    // Data members
};

} // namespace augr