#pragma once

#include <algorithm>
#include <string>
#include <utility>
#include <vector>

#include <augr/core/subject.h>

namespace augr {

enum class CreateMode { Fresh, Replicated, Loaded };

class Model : public Subject {
public:
    Model() {
        id_ = next_id_++;
    }
    virtual void Create()  {}
    virtual void OnFresh() {}  // called when creatd from scratch, not loaded
    virtual void OnReplicated() {}  // called when replicated from another instance, not created from scratch
    virtual void OnLoaded() {} // called after deserialization, not created from scratch

    template <class T, class... Args>
    static T& Make(Model* parent, CreateMode mode = CreateMode::Fresh, Args&&... args) {
        auto child = new T(std::forward<Args>(args)...);  // most-derived ctor runs
        T& ref = *child;
        ref.parent_ = parent;
        ref.create_mode_ = mode;
        ref.Create();
        if (parent)
            parent->AddChild(ref);  // fully built -> safe to publish + notify
        switch (mode) {
            case CreateMode::Fresh:
                ref.OnFresh();
                break;
            case CreateMode::Replicated:
                ref.OnReplicated();
                break;
            case CreateMode::Loaded:
                ref.OnLoaded();
                break;
        }
        return ref;
    }
    virtual void Destroy() {}
    void AddChild(Model &child) {
        OnAddingChild(child);
        children_.push_back(&child);
    }
    void RemoveChild(Model &child) {
        OnRemovingChild(child);
        children_.erase(std::remove(children_.begin(), children_.end(), &child),
                        children_.end());
    }
    // Accessors
    Model &parent() const { return *parent_; }

protected:
    virtual void OnAddingChild(Model &child) {}
    virtual void OnRemovingChild(Model &child) {}

public:
    // Data members
    Model *parent_ = nullptr;
    CreateMode create_mode_ = CreateMode::Fresh;
    std::vector<Model *> children_;

    static int next_id_;
    int id_ = 0;

    REFLECT_ENABLE(Subject)
};

} // namespace augr