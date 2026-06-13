// rack_doc.h
#pragma once

#include <filesystem>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>

#include <nlohmann/json.hpp>

#include <augr/core/document.h>

#include "rack.h"

namespace augr {

class RackDoc : public DocumentT<Rack> {
public:
    RackDoc() = default;
    ~RackDoc(); // out-of-line because Rack is forward-declared

    bool Save(const std::filesystem::path &p);
    void ReplaceRack(std::shared_ptr<Rack> fresh, nlohmann::json envelope);

    bool Load(const std::filesystem::path &p, bool auto_start = true);
    void NewDocument(bool auto_start = true);

    bool Start();
    void Stop();
    bool IsRunning() const;
};

} // namespace augr