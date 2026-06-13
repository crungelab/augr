#pragma once

#include "view.h"

namespace augr {

class Model;

class ModelView : public View {
public:
    // model: the initial target. Subclasses may retarget post-construction
    // via set_model() — but must do so before the first Build()/Draw().
    explicit ModelView(Model &model) : model_(&model) {}
    virtual ~ModelView() = default;

    Model *model() { return model_; }
    const Model *model() const { return model_; }

    // Retargets the view at a different model. Used by views like
    // SubrackView whose target is not known at the time DocumentViewT's
    // constructor runs (DocumentViewT initializes model_ to the doc's
    // root; SubrackViewer calls set_model() afterwards to point the view
    // at a specific nested subrack).
    //
    // Must be called before Build() runs, otherwise the widget tree
    // will reflect the wrong target.
    void set_model(Model &m) { model_ = &m; }

    virtual void Build();

    // Data members
    Model *model_ = nullptr;
    Widget* root_;
};

template <typename T, typename TBase = ModelView>
class ModelViewT : public TBase {
public:
    explicit ModelViewT(T &model) : TBase(model) {}

    T &model() { return *static_cast<T *>(this->model_); }
    const T &model() const { return *static_cast<const T *>(this->model_); }
};

} // namespace augr