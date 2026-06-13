#pragma once

#include <algorithm>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include <augr/core/subject.h>

namespace augr {

enum class CreateMode { Fresh, Replicated, Loaded };

class Model : public Subject, public std::enable_shared_from_this<Model> {
public:
    using Ptr = std::shared_ptr<Model>;
    using WeakPtr = std::weak_ptr<Model>;

    Model() { id_ = next_id_++; }
    Model(const std::string &label) : label_(label) { id_ = next_id_++; }
    virtual ~Model() = default;

    virtual void Create() {}
    virtual void OnFresh() {}
    virtual void OnReplicated() {}
    virtual void OnLoaded() {}

    template <class T, class... Args>
    static std::shared_ptr<T>
    Make(Ptr parent, CreateMode mode = CreateMode::Fresh, Args &&...args) {
        auto child = std::make_shared<T>(std::forward<Args>(args)...);
        child->parent_ = parent;
        child->create_mode_ = mode;
        child->Create();
        if (parent)
            parent->AddChild(child);
        switch (mode) {
        case CreateMode::Fresh:
            child->OnFresh();
            break;
        case CreateMode::Replicated:
            child->OnReplicated();
            break;
        case CreateMode::Loaded:
            child->OnLoaded();
            break;
        }
        return child;
    }

    template <class T, class... Args>
    static std::shared_ptr<T> MakeFresh(Ptr parent, Args &&...args) {
        return Make<T>(parent, CreateMode::Fresh, std::forward<Args>(args)...);
    }

    template <class T, class... Args>
    static std::shared_ptr<T> MakeReplicated(Ptr parent, Args &&...args) {
        return Make<T>(parent, CreateMode::Replicated,
                       std::forward<Args>(args)...);
    }

    template <class T, class... Args>
    static std::shared_ptr<T> MakeLoaded(Ptr parent, Args &&...args) {
        return Make<T>(parent, CreateMode::Loaded, std::forward<Args>(args)...);
    }

    void Destroy() {
        OnDestroy();
        if (auto p = parent_.lock())
            p->RemoveChild(*this);
    }

    virtual void OnDestroy() {}

    void AddChild(Ptr child) {
        OnAddingChild(*child);
        children_.push_back(std::move(child));
    }

    bool RemoveChild(Model &child) {
        auto it = std::find_if(children_.begin(), children_.end(),
                               [&](const Ptr &p) { return p.get() == &child; });
        if (it == children_.end())
            return false;
        OnRemovingChild(**it);
        //(*it)->Destroy();
        children_.erase(it);
        return true;
    }

    // Accessors
    const std::string &label() const { return label_; }
    void set_label(const std::string &label) { label_ = label; }

    Ptr parent() const { return parent_.lock(); }

    bool is_fresh() const { return create_mode_ == CreateMode::Fresh; }
    bool is_replicated() const {
        return create_mode_ == CreateMode::Replicated;
    }
    bool is_loaded() const { return create_mode_ == CreateMode::Loaded; }

protected:
    virtual void OnAddingChild(Model &child) {}
    virtual void OnRemovingChild(Model &child) {}

public:
    std::string label_;
    WeakPtr parent_;
    CreateMode create_mode_ = CreateMode::Fresh;
    std::vector<Ptr> children_;

    static int next_id_;
    int id_ = 0;

    REFLECT_ENABLE(Subject)
};

} // namespace augr