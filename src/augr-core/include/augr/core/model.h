#pragma once

#include <algorithm>
#include <string>
#include <utility>
#include <vector>

#include <augr/core/part.h>

namespace augr {

class Model : public Part {
public:
    Model() = default;
    void Create(Part *parent = nullptr) override {
        Part::Create(parent);
        if (parent) {
            dynamic_cast<Model *>(parent)->AddChild(*this);
        }
    }
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
    Model &parent() const { return *dynamic_cast<Model *>(owner_); }

protected:
    virtual void OnAddingChild(Model &child) {}
    virtual void OnRemovingChild(Model &child) {}

public:
    // Data members
    std::vector<Model *> children_;

    REFLECT_ENABLE(Part)
};

} // namespace augr