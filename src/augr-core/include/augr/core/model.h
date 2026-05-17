#pragma once

#include <algorithm>
#include <string>
#include <utility>
#include <vector>

#include <augr/core/subject.h>

namespace augr {

class Model : public Subject {
public:
    Model() {
        id_ = next_id_++;
    }
    virtual void Create(Model *parent = nullptr)  {
        parent_ = parent;
        if (parent) {
            parent->AddChild(*this);
        }
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
    std::vector<Model *> children_;

    static int next_id_;
    int id_ = 0;

    REFLECT_ENABLE(Subject)
};

} // namespace augr