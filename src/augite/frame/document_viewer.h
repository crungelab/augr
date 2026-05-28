#pragma once

#include <augr/core/document.h>

#include "viewer.h"

namespace augr {

class DocumentViewer : public Viewer {
public:
    DocumentViewer(const std::string &label, Document &doc) : Viewer(label), doc_(&doc) {}
    virtual ~DocumentViewer() = default;
    // Accessors
    Document &document() { return *doc_; }
    const Document &document() const { return *doc_; }
    // Data members
protected:
    Document *doc_;
};

template <typename TDoc, typename TView, typename TController, typename TBase = DocumentViewer>
class DocumentViewerT : public ViewerT<TView, TController, TBase> {
public:
    DocumentViewerT(const std::string &label, TDoc & doc) : ViewerT<TView, TController, TBase>(label, doc) {}
    // Accessors
    TDoc &document() { return *static_cast<TDoc *>(this->doc_); }
    const TDoc &document() const { return *static_cast<const TDoc *>(this->doc_); }
    //
    // Data members
};

} // namespace augr