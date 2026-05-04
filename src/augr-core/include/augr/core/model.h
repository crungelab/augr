#pragma once

#include <string>
#include <utility>
#include <vector>
#include <algorithm>

#include <augr/core/part.h>

namespace augr {

class Model : public Part {
public:
    Model() = default;
    void AddChild(Model &model) { children_.push_back(&model); }
    void RemoveChild(Model &model) {
        OnRemovingChild(model);
        children_.erase(std::remove(children_.begin(), children_.end(), &model),
                        children_.end());
    }
    // Accessors
    [[nodiscard]] Model &parent() const {
        return *dynamic_cast<Model *>(owner_);
    }
protected:
    virtual void OnRemovingChild(Model& model) {}
public:
    // Data members
    std::vector<Model *> children_;

    REFLECT_ENABLE(Part)
};

} // namespace augr