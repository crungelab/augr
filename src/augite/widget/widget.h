#pragma once

#include <typeindex>
#include <vector>
#include <algorithm>


namespace augr {

class Model;
class Widget;

class WidgetContainer {
public:
    virtual ~WidgetContainer() = default;
    void AddChild(Widget* widget) { children_.push_back(widget); }
    void RemoveChild(Widget &model) {
        children_.erase(std::remove(children_.begin(), children_.end(), &model),
                        children_.end());
    }
    // Data members
    std::vector<Widget *> children_;
};

class Widget : public WidgetContainer {
public:
    virtual void Draw() { DrawChildren(); }
    void DrawChildren() {
        for (auto child : children_)
            DrawChild(*child);
    }
    virtual void DrawChild(Widget &child) { child.Draw(); }
    // Accessors
    virtual Model *model() = 0;
    // Data members
};

template <typename T> class WidgetT : public Widget {
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
    std::type_index GetKey() override {
        return std::type_index(typeid(N));
    }
    // Data members
};

#define DEFINE_WIDGET_FACTORY(T, N)                                                    \
    WidgetFactoryT<T, N> T##Factory;                                           \
    WidgetFactory *Get##T##Factory() { return &T##Factory; }

} // namespace augr