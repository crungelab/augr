#pragma once

#include <algorithm>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include <uuid.h>

#include <augr/subject.h>

namespace augr {

enum class CreateMode { Fresh, Loaded, Copied, Replicated };

class Model : public Subject, public std::enable_shared_from_this<Model> {
public:
    using Ptr = std::shared_ptr<Model>;
    using WeakPtr = std::weak_ptr<Model>;

    Model() { id_ = next_id_++; }
    Model(const std::string &label) : label_(label) { id_ = next_id_++; }
    virtual ~Model();

    void Create(CreateMode mode = CreateMode::Fresh) {
        OnCreate();
        OnCreated();
        switch (mode) {
        case CreateMode::Fresh:
            OnCreateFresh();
            break;
        case CreateMode::Loaded:
            OnCreateLoaded();
            break;
        case CreateMode::Copied:
            OnCreateCopied();
            break;
        case CreateMode::Replicated:
            OnCreateReplicated();
            break;
        }
    }

    virtual void OnCreate() {}
    virtual void OnCreated() {}
    virtual void OnCreateFresh();
    virtual void OnCreateLoaded();
    virtual void OnCreateCopied();
    virtual void OnCreateReplicated();

    template <class T, class... Args>
    static std::shared_ptr<T>
    Make(Ptr parent, CreateMode mode = CreateMode::Fresh, Args &&...args) {
        auto child = std::make_shared<T>(std::forward<Args>(args)...);
        // parent_ is set early so OnCreateFresh et al. can see it; AddChild
        // (re)sets it to the same value when the child is registered.
        child->parent_ = parent;
        child->create_mode_ = mode;
        child->Create(mode);
        if (parent)
            parent->AddChild(child);
        return child;
    }

    template <class T, class... Args>
    static std::shared_ptr<T> MakeFresh(Ptr parent, Args &&...args) {
        return Make<T>(parent, CreateMode::Fresh, std::forward<Args>(args)...);
    }

    template <class T, class... Args>
    static std::shared_ptr<T> MakeLoaded(Ptr parent, Args &&...args) {
        return Make<T>(parent, CreateMode::Loaded, std::forward<Args>(args)...);
    }

    template <class T, class... Args>
    static std::shared_ptr<T> MakeCopied(Ptr parent, Args &&...args) {
        return Make<T>(parent, CreateMode::Copied, std::forward<Args>(args)...);
    }

    template <class T, class... Args>
    static std::shared_ptr<T> MakeReplicated(Ptr parent, Args &&...args) {
        return Make<T>(parent, CreateMode::Replicated,
                       std::forward<Args>(args)...);
    }

    void Destroy() {
        OnDestroy();
        if (auto p = parent_.lock())
            p->RemoveChild(*this);
    }

    virtual void OnDestroy() {}

    // Registers child under this model, establishing the parent link. Fires
    // OnAddingChild before the child is visible in children_, matching the
    // ordering OnRemovingChild sees on the way out.
    void AddChild(Ptr child) {
        child->parent_ = weak_from_this();
        OnAddingChild(*child);
        children_.push_back(std::move(child));
    }

    // Removes child and destroys it (the returned owning pointer is dropped).
    void RemoveChild(Model &child) { DetachChild(child); }

    // Removes child WITHOUT destroying it and returns the owning pointer, so
    // the caller can re-home it. Fires OnRemovingChild while the child is
    // still in children_ and its parent link is still valid, then clears the
    // parent link. Returns nullptr if child isn't actually ours.
    Ptr DetachChild(Model &child) {
        auto it = std::find_if(children_.begin(), children_.end(),
                               [&](const Ptr &p) { return p.get() == &child; });
        if (it == children_.end())
            return nullptr;

        Ptr detached = *it; // hold a reference so the erase can't free it
        OnRemovingChild(*detached);
        children_.erase(it);
        detached->parent_.reset();
        return detached;
    }

    // Moves this model to new_parent without a destroy/create cycle. Composes
    // directly from DetachChild + AddChild, so both parents' bookkeeping hooks
    // (OnRemovingChild / OnAddingChild) fire exactly as in a normal move, and
    // the parent link is maintained by AddChild. No-op if new_parent is null,
    // there is no current parent, or new_parent is already the parent.
    void Reparent(Ptr new_parent) {
        if (!new_parent)
            return;
        auto old_parent = parent_.lock();
        if (!old_parent || old_parent.get() == new_parent.get())
            return;

        if (Ptr self = old_parent->DetachChild(*this))
            new_parent->AddChild(std::move(self));
    }

    Model *FindByUuid(const uuids::uuid &uuid);

    // Accessors
    uuids::uuid uuid() const { return uuid_; }
    std::string uuid_to_string() const { return uuids::to_string(uuid_); }
    void set_uuid(const uuids::uuid &u) { uuid_ = u; }
    void set_uuid(const std::string &s) {
        uuid_ = uuids::uuid::from_string(s).value_or(uuids::uuid{});
    }

    const std::string &label() const { return label_; }
    void set_label(const std::string &label) { label_ = label; }

    Ptr parent() const { return parent_.lock(); }

    bool is_fresh() const { return create_mode_ == CreateMode::Fresh; }
    bool is_loaded() const { return create_mode_ == CreateMode::Loaded; }
    bool is_replicated() const {
        return create_mode_ == CreateMode::Replicated;
    }
    bool is_copied() const { return create_mode_ == CreateMode::Copied; }

protected:
    virtual void OnAddingChild(Model &child) {}
    virtual void OnRemovingChild(Model &child) {}

    // Data members
public:
    uuids::uuid uuid_;
    std::string label_;
    WeakPtr parent_;
    CreateMode create_mode_ = CreateMode::Fresh;
    std::vector<Ptr> children_;

    static int next_id_;
    int id_ = 0;

    REFLECT_ENABLE(Subject)
};

} // namespace augr