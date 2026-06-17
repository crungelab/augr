#pragma once

#include "../view/model_view.h"
#include "controller.h"

namespace augr {

class Model;

class ModelController : public Controller {
public:
    ModelController(Model &model, ModelView &view, Frame &frame)
        : Controller(frame), model_(&model), view_(&view) {}

    Model &model() { return *model_; }
    const Model &model() const { return *model_; }

    void set_model(Model &m) { model_ = &m; }

    ModelView &view() { return *view_; }
    const ModelView &view() const { return *view_; }

    Model *model_;
    ModelView *view_;
};

} // namespace augr