#pragma once

#include <augr/core/document.h>

#include "../controller/controller.h"
#include "../view/view.h"
#include "../widget/dock.h"

namespace augr {

class Frame : public Dock {
public:
    //Frame(const std::string &label = "") : Dock(label) {}
    Frame(Document &doc, const std::string &label = "") : Dock(label), doc_(&doc) {}
    virtual ~Frame() = default;
    //void Draw() override;
    void End() override;
    // Accessors
    Document &doc() { return *doc_; }
    const Document &doc() const { return *doc_; }
    View &view() { return *view_; }
    const View &view() const { return *view_; }
    Controller &controller() { return *controller_; }
    const Controller &controller() const { return *controller_; }

    bool is_active();
    // Data members
protected:
    //std::unique_ptr<Document> doc_;
    Document *doc_;
    std::unique_ptr<View> view_;
    std::unique_ptr<Controller> controller_;
};

template <typename TDoc, typename TView, typename TController>
class FrameT : public Frame {
public:
    //FrameT(const std::string &label = "") : Frame(label) {}
    FrameT(TDoc & doc, const std::string &label = "") : Frame(doc, label) {}
    // Accessors
    TDoc &doc() { return *static_cast<TDoc *>(doc_); }
    const TDoc &doc() const { return *static_cast<const TDoc *>(doc_); }
    TView &view() { return *static_cast<TView *>(view_.get()); }
    TController &controller() {
        return *static_cast<TController *>(controller_.get());
    }
    //
    // Data members
};

} // namespace augr