#pragma once

#include <augr/core/document.h>

#include "rack.h"

namespace augr {

class RackDoc : public Document {
public:
    Rack &rack() { return *rack_; } // RackView gets a ref to this

    bool Save(const std::filesystem::path &p) override;
    bool Load(const std::filesystem::path &p) override;

    void NewDocument() override {
        ClearPath();
        MarkClean();
    }

    std::string TypeName() const override { return "Augr Rack"; }
    std::vector<std::string> Extensions() const override { return {".augr"}; }

protected:
    Rack *rack_;
};

} // namespace augr