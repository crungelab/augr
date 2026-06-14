#pragma once

#include <sigslot/signal.hpp>

#include <augr/core/document.h>

#include "../controller/controller.h"
#include "../view/view.h"

#include "../widget/frame.h"

namespace augr {

class Viewer : public Frame {
public:
    friend class ViewerArchiver;

    Viewer(const std::string &label, Document &doc, Model &model);
    virtual ~Viewer() {}

    void Create() override;
    void OnDestroy() override;

    virtual void OnLoaded();
    virtual void RebuildView() {}

    void Begin() override;
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

    // View serialization (called from doc hooks).
    void SaveViewerState();
    void RestoreViewerState();

    sigslot::scoped_connection on_doc_save_conn_;
    sigslot::scoped_connection on_doc_load_conn_;
};

template <typename TDoc, typename TModel, typename TView, typename TController = Controller,
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

} // namespace augr