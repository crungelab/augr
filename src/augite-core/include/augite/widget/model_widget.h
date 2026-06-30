#pragma once

#include "widget.h"

namespace augr {

// A widget bound to a Model. Adds the model() accessor that callers
// (serialization, parameter binding, graph wiring, etc.) rely on.
class ModelWidget : public Widget {
public:
    ModelWidget(Model &model) : model_(&model) {}
    // using Ptr = std::unique_ptr<ModelWidget>;
    //  Accessors
    Model &model() { return *model_; }
    const Model &model() const { return *model_; }
    // Data members
    Model *model_;
};

// CRTP-ish helper: stores a typed pointer to the concrete model and
// satisfies the ModelWidget::model() contract. TBase defaults to
// ModelWidget but can be overridden to inject intermediate classes
// in the inheritance chain.
template <typename T, typename TBase = ModelWidget>
class ModelWidgetT : public TBase {
public:
    ModelWidgetT(T &model) : TBase(model) {}
    T &model() { return *static_cast<T *>(TBase::model_); }
    const T &model() const { return *static_cast<const T *>(TBase::model_); }
};

// Factory for ModelWidget instances. Keyed on the model type so the
// UI builder can look up the right widget to produce for a given
// model at runtime.
class ModelWidgetFactory {
public:
    virtual ~ModelWidgetFactory() = default;
    // Returns an owned widget. Caller is responsible for inserting it
    // into the widget tree (typically via parent->AddChild(...)).
    virtual ModelWidget::Ptr Produce(Model &model) = 0;
    virtual std::type_index GetKey() = 0;
    // Accessors
    const std::string &name() const { return name_; }
    // Data members
private:
    std::string name_;
};

template <typename T, typename N = Model>
class ModelWidgetFactoryT : public ModelWidgetFactory {
    ModelWidget::Ptr Produce(Model &model) override {
        return std::make_unique<T>(static_cast<N &>(model));
    }
    std::type_index GetKey() override { return std::type_index(typeid(N)); }
};

#define DEFINE_MODEL_WIDGET_FACTORY(T, N)                                      \
    ModelWidgetFactoryT<T, N> T##Factory;                                      \
    ModelWidgetFactory *Get##T##Factory() { return &T##Factory; }

} // namespace augr