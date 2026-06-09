#pragma once

#include <augr/core/document.h>

#include "viewer.h"

namespace augr {

class DocumentViewer : public Viewer {
public:
    DocumentViewer(const std::string &label, Document &doc) : Viewer(label, doc) {}
    virtual ~DocumentViewer() = default;
    // Accessors
    // Data members
protected:
};

template <typename TDoc, typename TView, typename TController, typename TBase = DocumentViewer>
class DocumentViewerT : public ViewerT<TDoc, TView, TController, TBase> {
public:
    DocumentViewerT(const std::string &label, TDoc & doc) : ViewerT<TDoc, TView, TController, TBase>(label, doc) {}
    // Accessors
    // Data members
};

} // namespace augr