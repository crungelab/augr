#pragma once

#include <augr/core/document.h>

#include "../controller/controller.h"
#include "../view/view.h"

#include "../widget/frame.h"

namespace augr {

class Viewer : public Frame {
public:
    Viewer(const std::string &label, Document &doc, Model &model)
        : Frame(label), doc_(&doc), model_(&model) {}
    virtual ~Viewer() = default;

    // void Draw() override;
    void End() override;

    // Accessors
    Document &document() { return *doc_; }
    const Document &document() const { return *doc_; }

    void set_model(Model &model) { model_ = &model; }
    Model &model() { return *model_; }
    const Model &model() const { return *model_; }

    View &view() { return *view_; }
    const View &view() const { return *view_; }

    Controller &controller() { return *controller_; }
    const Controller &controller() const { return *controller_; }

    // Data members
protected:
    Document *doc_;
    Model *model_;
    std::unique_ptr<View> view_;
    std::unique_ptr<Controller> controller_;
};

template <typename TDoc, typename TModel, typename TView, typename TController,
          typename TBase = Viewer>
class ViewerT : public TBase {
public:
    template <typename... Args>
    ViewerT(const std::string &label, TDoc &doc, TModel &model, Args &&...args)
        : TBase(label, doc, model) {}
    // Accessors
    TDoc &document() { return *static_cast<TDoc *>(this->doc_); }
    const TDoc &document() const {
        return *static_cast<const TDoc *>(this->doc_);
    }

    TModel &model() { return *static_cast<TModel *>(this->model_); }
    const TModel &model() const {
        return *static_cast<const TModel *>(this->model_);
    }

    TView &view() { return *static_cast<TView *>(this->view_.get()); }
    const TView &view() const {
        return *static_cast<const TView *>(this->view_.get());
    }

    TController &controller() {
        return *static_cast<TController *>(this->controller_.get());
    }
    const TController &controller() const {
        return *static_cast<const TController *>(this->controller_.get());
    }
    //
    // Data members
};

// Factory for Viewer instances. Keyed on the model type so the
// UI builder can look up the right viewer to produce for a given
// model at runtime.
class ViewerFactory {
public:
    virtual ~ViewerFactory() = default;
    // Returns an owned widget. Caller is responsible for inserting it
    // into the widget tree (typically via parent->AddChild(...)).
    virtual Viewer::Ptr Produce(const std::string &label, Document &doc,
                                Model &model) = 0;
    virtual std::type_index GetKey() = 0;
};

template <typename T, typename N = Model>
class ViewerFactoryT : public ViewerFactory {
    Viewer::Ptr Produce(const std::string &label, Document &doc,
                        Model &model) override {
        return std::make_unique<T>(label, doc, static_cast<N &>(model));
    }
    std::type_index GetKey() override { return std::type_index(typeid(N)); }
};

#define DEFINE_VIEWER_FACTORY(T, N)                                            \
    ViewerFactoryT<T, N> T##Factory;                                           \
    ViewerFactory *Get##T##Factory() { return &T##Factory; }

} // namespace augr