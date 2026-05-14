// rack_doc.h (sketch — adjust to your actual layout)
#pragma once

#include <filesystem>
#include <memory>
#include <optional>
#include <string>

#include <augr/core/document.h>

#include "rack.h"

namespace augr {

//class Rack;

class RackDoc : public DocumentT<Rack> {
public:
    RackDoc() = default;
    ~RackDoc(); // out-of-line because Rack is forward-declared

    bool Save(const std::filesystem::path &p);
    bool Load(const std::filesystem::path& p, bool auto_start = true);
    void NewDocument(bool auto_start = true);

    // Audio lifecycle for the current rack.
    bool Start();          // returns false if no rack or start failed
    void Stop();
    bool IsRunning() const;

    Rack &rack() { return *model_; }
    const Rack &rack() const { return *model_; }
    bool HasRack() const { return model_ != nullptr; }
};

} // namespace augr