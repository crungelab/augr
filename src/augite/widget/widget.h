#pragma once

#include <algorithm>
#include <typeindex>
#include <vector>

#include <augr/core/subject.h>

namespace augr {

class Model;

// Base widget: a node in the UI tree. Knows how to draw itself and its
// children, but has no inherent association with a Model. Use this for
// pure layout/structural widgets (containers, dividers, group boxes,
// static labels, etc.).
class Widget : public Subject {
public:
    virtual ~Widget() = default;
    virtual void Create(Widget *parent = nullptr) {
        if (parent) {
            parent->AddChild(this);
        }
    }
    void AddChild(Widget *widget) {
        widget->parent_ = this;
        children_.push_back(widget);
    }
    void RemoveChild(Widget &child) {
        child.parent_ = nullptr;
        children_.erase(std::remove(children_.begin(), children_.end(), &child),
                        children_.end());
    }

    virtual void Draw() { DrawChildren(); }
    void DrawChildren() {
        for (auto child : children_)
            DrawChild(*child);
    }
    virtual void DrawChild(Widget &child) { child.Draw(); }

    // Data members
    Widget *parent_ = nullptr;
    std::vector<Widget *> children_;
};

// A widget bound to a Model. Adds the model() accessor that callers
// (serialization, parameter binding, graph wiring, etc.) rely on.
class ModelWidget : public Widget {
public:
    virtual Model *model() = 0;
};

// CRTP-ish helper: stores a typed pointer to the concrete model and
// satisfies the ModelWidget::model() contract. TBase defaults to
// ModelWidget but can be overridden to inject intermediate classes
// in the inheritance chain.
template <typename T, typename TBase = ModelWidget>
class ModelWidgetT : public TBase {
public:
    ModelWidgetT(T &model) : model_(&model) {}
    Model *model() override { return model_; }

    // Data members
    T *model_;
};

// Factory for ModelWidget instances. Keyed on the model type so the
// UI builder can look up the right widget to produce for a given
// model at runtime.
class ModelWidgetFactory {
public:
    virtual ~ModelWidgetFactory() = default;
    virtual ModelWidget *Produce(Model &model) = 0;
    virtual std::type_index GetKey() = 0;
};

template <typename T, typename N = Model>
class ModelWidgetFactoryT : public ModelWidgetFactory {
    ModelWidget *Produce(Model &model) override { return new T((N &)model); }
    std::type_index GetKey() override { return std::type_index(typeid(N)); }
};

#define DEFINE_MODEL_WIDGET_FACTORY(T, N)                                      \
    ModelWidgetFactoryT<T, N> T##Factory;                                      \
    ModelWidgetFactory *Get##T##Factory() { return &T##Factory; }

} // namespace augr