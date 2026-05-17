// rack_doc.h
#pragma once

#include <filesystem>
#include <memory>
#include <optional>
#include <string>

#include <nlohmann/json.hpp>

#include <augr/core/document.h>

#include "rack.h"

namespace augr {

class RackDoc : public DocumentT<Rack> {
 public:
    RackDoc() = default;
    ~RackDoc(); // out-of-line because Rack is forward-declared

    bool Save(const std::filesystem::path &p);
    bool Load(const std::filesystem::path &p, bool auto_start = true);
    void NewDocument(bool auto_start = true);

    bool Start();
    void Stop();
    bool IsRunning() const;

    Rack &rack() { return *model_; }
    const Rack &rack() const { return *model_; }
    bool HasRack() const { return model_ != nullptr; }

    // Document-level metadata. Anything not part of the rack itself lives
    // here: editor state, user notes, MIDI-learn maps, etc. Stored as raw
    // JSON so subsystems can own their own subkeys without RackDoc needing
    // to know the schema.
    nlohmann::json &metadata() { return metadata_; }
    const nlohmann::json &metadata() const { return metadata_; }

    // Convenience: get/set a top-level metadata subobject by key. Useful
    // for "editor", "midi_learn", etc. without callers having to remember
    // to ensure-object on first write.
    nlohmann::json &MetadataSection(const std::string &key);

 private:
    nlohmann::json metadata_ = nlohmann::json::object();
};

} // namespace augr