// rack_doc.h (sketch — adjust to your actual layout)
#pragma once

#include <filesystem>
#include <memory>
#include <optional>
#include <string>

namespace augr {

class Rack;

class RackDoc {
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

    Rack &rack() { return *rack_; }
    const Rack &rack() const { return *rack_; }
    bool HasRack() const { return rack_ != nullptr; }

    const std::optional<std::filesystem::path> &Path() const { return path_; }
    bool IsModified() const { return modified_; }
    void MarkModified() { modified_ = true; }

    std::string DisplayName() const {
        std::string base = path_ ? path_->filename().string() : "Untitled";
        return modified_ ? base + "*" : base;
    }

protected:
    void SetPath(std::filesystem::path p) { path_ = std::move(p); }
    void ClearPath() { path_.reset(); }
    void MarkClean() { modified_ = false; }

private:
    std::unique_ptr<Rack> rack_;
    std::optional<std::filesystem::path> path_;
    bool modified_ = false;
};

} // namespace augr