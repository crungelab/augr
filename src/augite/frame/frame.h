#pragma once

#include <augr/core/document.h>

#include "../view/view.h"

#include "../widget/dock.h"

namespace augr {

class Frame : public Dock {
public:
    Frame(const std::string &label = "") : Dock(label) {}
    void Draw() override;
    // Data members
    std::unique_ptr<Document> doc_;
    std::unique_ptr<View> view_;
};

template <typename TDoc, typename TView> class FrameT : public Frame {
public:
    FrameT(const std::string &label = "") : Frame(label) {}
    // Accessors
    TDoc &doc() { return *static_cast<TDoc *>(doc_.get()); }
    TView &view() { return *static_cast<TView *>(view_.get()); }
    //
    // Data members
};

} // namespace augr