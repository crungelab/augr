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

    void ClearViewers() { viewers_.clear(); }

    // Document-level metadata. Anything not part of the rack itself lives
    // here: editor state, user notes, MIDI-learn maps, etc. Stored as raw
    // JSON so subsystems can own their own subkeys without RackDoc needing
    // to know the schema.
    nlohmann::json &metadata() { return metadata_; }
    const nlohmann::json &metadata() const { return metadata_; }

    // Per-subrack archived view state, keyed by Subrack::uuid().
    // Populated when SubrackFrames close (or at save time for open
    // frames), consumed when SubrackFrames open. Serialized as part
    // of the document envelope.
    std::unordered_map<std::string, nlohmann::json> viewers_;

private:
    nlohmann::json metadata_ = nlohmann::json::object();
};

} // namespace augr