#pragma once

#include "model_view.h"

#include <augr/core/document.h>

namespace augr {

//class Document;

class DocumentView : public ModelView {
public:
    virtual Document *document() = 0;
};

template <typename T> class DocumentViewT : public DocumentView {
public:
    DocumentViewT(T &doc) : doc_(&doc) {}
    virtual Model *model() override { return doc_->model(); }
    virtual Document *document() override { return doc_; }
    // Data members
    T *doc_;
};

} // namespace augr