#pragma once

#include <algorithm>
#include <typeindex>
#include <vector>

namespace augr {

class Model;

class Widget {
public:
    virtual ~Widget() = default;
    void AddChild(Widget *widget) { children_.push_back(widget); }
    void RemoveChild(Widget &model) {
        children_.erase(std::remove(children_.begin(), children_.end(), &model),
                        children_.end());
    }

    virtual void Draw() { DrawChildren(); }
    void DrawChildren() {
        for (auto child : children_)
            DrawChild(*child);
    }
    virtual void DrawChild(Widget &child) { child.Draw(); }
    // Accessors
    //virtual Model *model() = 0;
    virtual Model *model() { return nullptr; }
    // Data members
    std::vector<Widget *> children_;
};

template <typename T, typename TBase = Widget> class WidgetT : public TBase {
public:
    WidgetT(T &model) : model_(&model) {}
    Model *model() override { return model_; }
    // Data members
    T *model_;
};

// Factory
class WidgetFactory {
public:
    virtual Widget *Produce(Model &model) = 0;
    virtual std::type_index GetKey() = 0;
};

template <typename T, typename N = Model>
class WidgetFactoryT : public WidgetFactory {
    Widget *Produce(Model &model) override { return new T((N &)model); }
    std::type_index GetKey() override { return std::type_index(typeid(N)); }
    // Data members
};

#define DEFINE_WIDGET_FACTORY(T, N)                                            \
    WidgetFactoryT<T, N> T##Factory;                                           \
    WidgetFactory *Get##T##Factory() { return &T##Factory; }

} // namespace augr