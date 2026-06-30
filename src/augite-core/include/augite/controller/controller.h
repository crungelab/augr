#pragma once

namespace augr {

class Document;
class Model;
class View;
class Frame;

class Controller {
public:
    Controller(Document &doc, Model &model, View &view, Frame &frame)
        : doc_(&doc), model_(&model), view_(&view), frame_(&frame) {}
    virtual ~Controller() = default;
    virtual void Control() {}

    // Accessors
    Frame &frame() { return *frame_; }
    const Frame &frame() const { return *frame_; }
    // Data members
protected:
    Document *doc_;
    Model *model_;
    View *view_;
    Frame *frame_;
};

template <typename TDoc, typename TModel, typename TView>
class ControllerT : public Controller {
public:
    using Controller::Controller;

    // Accessors
    TDoc &document() { return *static_cast<TDoc *>(this->doc_); }
    const TDoc &document() const {
        return *static_cast<const TDoc *>(this->doc_);
    }

    TModel &model() { return *static_cast<TModel *>(this->model_); }
    const TModel &model() const {
        return *static_cast<const TModel *>(this->model_);
    }

    TView &view() { return *static_cast<TView *>(this->view_); }
    const TView &view() const {
        return *static_cast<const TView *>(this->view_);
    }
};


} // namespace augr