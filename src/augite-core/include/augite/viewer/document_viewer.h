#pragma once

#include <augr/document.h>

#include "viewer.h"

namespace augr {

/*
 * DocumentViewer is a Viewer that's model is the same as the document's model
 * It is responsible for showing the menubar for the document
 */

class DocumentViewer : public Viewer {
public:
    DocumentViewer(const std::string &label, Document &doc, Model &model)
        : Viewer(label, doc, model) {}
    virtual ~DocumentViewer() = default;
    // Accessors
    // Data members
protected:
};

template <typename TDoc, typename TModel, typename TView, typename TController,
          typename TBase = DocumentViewer>
class DocumentViewerT
    : public ViewerT<TDoc, TModel, TView, TController, TBase> {
public:
    DocumentViewerT(const std::string &label, TDoc &doc, TModel &model)
        : ViewerT<TDoc, TModel, TView, TController, TBase>(label, doc, model) {}

    void RebuildView() override {
        this->view_ = std::make_unique<TView>(this->model());
        this->view().Build();

        this->controller_ =
            std::make_unique<TController>(this->document(), this->model(), this->view(), *this);
    }
    // Accessors
    // Data members
};

} // namespace augr