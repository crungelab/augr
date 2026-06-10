#pragma once

#include <augr/core/document.h>

#include "viewer.h"

namespace augr {

class DocumentViewer : public Viewer {
public:
    DocumentViewer(const std::string &label, Document &doc, Model &model) : Viewer(label, doc, model) {}
    virtual ~DocumentViewer() = default;
    // Accessors
    // Data members
protected:
};

template <typename TDoc, typename TModel, typename TView, typename TController, typename TBase = DocumentViewer>
class DocumentViewerT : public ViewerT<TDoc, TModel, TView, TController, TBase> {
public:
    DocumentViewerT(const std::string &label, TDoc & doc, TModel &model) : ViewerT<TDoc, TModel, TView, TController, TBase>(label, doc, model) {}
    // Accessors
    // Data members
};

} // namespace augr