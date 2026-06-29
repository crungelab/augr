#pragma once

#include <augr/model.h>

#include "viewer.h"

namespace augr {

class ModelViewer : public Viewer {
public:
    ModelViewer(const std::string &label, Document &doc, Model &model) : Viewer(label, doc, model) {}
    virtual ~ModelViewer() = default;
    // Accessors
    // Data members
protected:
};

template <typename TDoc, typename TModel, typename TView, typename TController = Controller, typename TBase = ModelViewer>
class ModelViewerT : public ViewerT<TDoc, TModel, TView, TController, TBase> {
public:
    ModelViewerT(const std::string &label, TDoc & doc, TModel &model) : ViewerT<TDoc, TModel, TView, TController, TBase>(label, doc, model) {}

    void RebuildView() override {
        this->view_ = std::make_unique<TView>(this->model());
        this->view().Build();
    }
    // Accessors
    // Data members
};

} // namespace augr