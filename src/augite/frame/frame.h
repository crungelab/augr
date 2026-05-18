#pragma once

#include <augr/core/document.h>

#include "../controller/controller.h"
#include "../view/view.h"
#include "../widget/dock.h"

namespace augr {

class Frame : public Dock {
public:
    Frame(const std::string &label = "") : Dock(label) {}
    virtual ~Frame() = default;
    void Draw() override;
    // Data members
protected:
    std::unique_ptr<Document> doc_;
    std::unique_ptr<View> view_;
    std::unique_ptr<Controller> controller_;
};

template <typename TDoc, typename TView, typename TController>
class FrameT : public Frame {
public:
    FrameT(const std::string &label = "") : Frame(label) {}
    // Accessors
    TDoc &doc() { return *static_cast<TDoc *>(doc_.get()); }
    TView &view() { return *static_cast<TView *>(view_.get()); }
    TController &controller() {
        return *static_cast<TController *>(controller_.get());
    }
    //
    // Data members
};

} // namespace augr