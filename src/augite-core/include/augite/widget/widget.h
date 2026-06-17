#pragma once

#include <algorithm>
#include <memory>
#include <typeindex>
#include <utility>
#include <vector>

#include <imgui.h>

#include <augr/subject.h>
#include <augr/math/vec2.h>

namespace augr {

class Model;
class Widget;

// Forward declaration of the deletion queue interface. Actual definition
// lives with App; Widget only needs to schedule itself for destruction.
class DestroyQueue {
public:
    virtual ~DestroyQueue() = default;
    virtual void ScheduleDestroy(std::unique_ptr<Widget> widget) = 0;
};

// Returns the process-wide destroy queue. Typically forwards to
// App::singleton().destroy_queue(). Defined in the .cpp / App module
// so widget.h doesn't have to know about App.
DestroyQueue &GetDestroyQueue();

// Base widget: a node in the UI tree. Knows how to draw itself and its
// children, but has no inherent association with a Model. Use this for
// pure layout/structural widgets (containers, dividers, group boxes,
// static labels, etc.).
//
// Ownership model: a parent owns its children via std::unique_ptr.
// Top-level widgets (Frames) are owned by App. Destruction is
// deferred: Destroy() detaches the widget from its parent synchronously
// (so it stops drawing and receiving events immediately) and hands
// ownership to the destroy queue, which deletes it at a safe point
// in the main loop.
class Widget : public Subject {
public:
    using Ptr = std::unique_ptr<Widget>;

    virtual ~Widget() = default;

    // Attach this widget to a parent. Caller must have allocated *this
    // with `new` and must not retain ownership after this call — the
    // parent takes ownership. If parent is null, the widget is left as
    // an orphan; the caller is responsible for handing ownership to App
    // (for top-level widgets) or destroying it.
    virtual void Create() {
    }

    void AddChild(Ptr child) {
        child->parent_ = this;
        children_.push_back(std::move(child));
    }

    // Detach a child and return ownership to the caller. Returns nullptr
    // if `child` is not actually a child of this widget.
    Ptr RemoveChild(Widget &child) {
        auto it = std::find_if(children_.begin(), children_.end(),
                               [&](const Ptr &p) { return p.get() == &child; });
        if (it == children_.end())
            return nullptr;
        Ptr owned = std::move(*it);
        children_.erase(it);
        owned->parent_ = nullptr;
        return owned;
    }

    // Called synchronously at the start of Destroy() and DestroyChildren(),
    // before the widget is detached from its parent or queued for deletion.
    // Override to disconnect signals, release external resources, etc.
    // Guaranteed to be called exactly once. The default is a no-op.
    virtual void OnDestroy() {}

    // Schedule this widget (and its subtree) for destruction at the next
    // safe point in the main loop. Detaches from the parent immediately
    // so the widget stops being drawn or receiving events. Safe to call
    // from inside the widget's own event handlers.
    // Non-virtual: override OnDestroy() instead.
    void Destroy() {
        if (destroying_)
            return;
        destroying_ = true;
        OnDestroy();

        Ptr self;
        if (parent_) {
            self = parent_->RemoveChild(*this);
        }
        // If parent_ is null, this is either a top-level widget (App
        // should arrange ScheduleDestroy directly, see Frame) or an orphan
        // the caller already owns. In the latter case the caller must
        // arrange for ScheduleDestroy itself; we can't take ownership
        // of `this` without a unique_ptr.
        if (self) {
            GetDestroyQueue().ScheduleDestroy(std::move(self));
        }
    }

    // Destroy a specific child. Equivalent to child.Destroy() but reads
    // more naturally at the call site when you have the parent in hand.
    // Returns true if `child` was actually a child of this widget.
    bool DestroyChild(Widget &child) {
        if (child.parent_ != this)
            return false;
        child.Destroy();
        return true;
    }

    // Destroy all children. Drains the children vector in one shot and
    // schedules each child for deletion directly, bypassing the usual
    // Destroy() -> RemoveChild path to avoid O(n^2) erasure and
    // iterator invalidation while mutating children_ mid-loop.
    void DestroyChildren() {
        std::vector<Ptr> doomed;
        doomed.swap(children_);
        auto &queue = GetDestroyQueue();
        for (auto &child : doomed) {
            if (child->destroying_)
                continue;
            child->destroying_ = true;
            child->OnDestroy();
            child->parent_ = nullptr;
            queue.ScheduleDestroy(std::move(child));
        }
    }

    virtual void Draw() {
        if (destroying_)
            return;
        DrawChildren();
    }

    void DrawChildren() {
        for (auto &child : children_)
            DrawChild(*child);
    }

    virtual void DrawChild(Widget &child) {
        /*
        if (child.destroying_)
            return;
        */
        child.Draw(); 
    }

    // Conversion helpers — keep Vec2 (ImGui-free) at the data layer
    // and only touch ImVec2 at the draw boundary.
    static ImVec2 ToImVec2(const Vec2 &v) { return ImVec2(v.x, v.y); }
    static Vec2 FromImVec2(const ImVec2 &v) { return Vec2{v.x, v.y}; }

    // Accessors
    std::vector<Widget *> child_pointers() const {
        std::vector<Widget *> result;
        result.reserve(children_.size());
        for (const auto &c : children_)
            result.push_back(c.get());
        return result;
    }
    bool destroying() const { return destroying_; }

    // Data members
    Widget *parent_ = nullptr;
    std::vector<Ptr> children_;
    bool destroying_ = false;
};

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