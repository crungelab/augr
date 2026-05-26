#pragma once

#include "model_view.h"

#include <augr/core/document.h>

namespace augr {

class DocumentView : public ModelView {
public:
    explicit DocumentView(Document &doc)
        : ModelView(*doc.model()), doc_(&doc) {}

    Document *document() { return doc_; }
    const Document *document() const { return doc_; }

    // Data members
    Document *doc_;
};

template <typename T> class DocumentViewT : public DocumentView {
public:
    explicit DocumentViewT(T &doc) : DocumentView(doc) {}

    T *document() { return static_cast<T *>(doc_); }
    const T *document() const { return static_cast<const T *>(doc_); }
};

} // namespace augr