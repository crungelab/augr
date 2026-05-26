#pragma once

#include "model_controller.h"

namespace augr {

template <typename TDoc, typename TView>
class DocumentController : public ModelController {
public:
    DocumentController(TDoc &doc, TView &view, Frame &frame)
        : ModelController(*doc.model(), view, frame), doc_(&doc) {}

    TDoc &document() { return *doc_; }
    const TDoc &document() const { return *doc_; }

    TView &view() { return *static_cast<TView *>(view_); }
    const TView &view() const { return *static_cast<const TView *>(view_); }

    TDoc *doc_;
};

} // namespace augr