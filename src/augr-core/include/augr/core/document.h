#pragma once

#include <filesystem>
#include <optional>
#include <string>
#include <vector>

#include <nlohmann/json.hpp>

namespace augr {

class Model;

class Document {
public:
    virtual ~Document() = default;
    virtual Model *model() = 0;
    /*
    virtual bool Save(const std::filesystem::path&) = 0;
    virtual bool Load(const std::filesystem::path&) = 0;
    virtual void NewDocument() = 0;
    virtual std::string TypeName() const = 0;        // "Rack", for title bar / file filters
    virtual std::vector<std::string> Extensions() const = 0;  // {".augr"}
    */

    const std::optional<std::filesystem::path>& Path() const { return path_; }
    bool IsModified() const { return modified_; }
    void MarkModified() { modified_ = true; }

    std::string DisplayName() const {
        std::string base = path_ ? path_->filename().string() : "Untitled";
        return modified_ ? base + "*" : base;
    }

    // Hooks let subsystems persist data alongside the rack without
    // RackDoc knowing about them. Each hook owns a top-level JSON key
    // ("view", "midi_learn", etc.) and is invoked during Save/Load.
    using SaveHookFn = std::function<nlohmann::json()>;
    using LoadHookFn = std::function<void(const nlohmann::json&)>;

    // Returns a token that can be used to remove the hook later.
    using HookToken = int;
    HookToken AddSaveHook(std::string key, SaveHookFn fn);
    HookToken AddLoadHook(std::string key, LoadHookFn fn);

    void RemoveSaveHook(HookToken token);
    void RemoveLoadHook(HookToken token);

protected:
    void SetPath(std::filesystem::path p) { path_ = std::move(p); }
    void ClearPath() { path_.reset(); }
    void MarkClean() { modified_ = false; }

    std::optional<std::filesystem::path> path_;
    bool modified_ = false;

    struct SaveHook { HookToken token; std::string key; SaveHookFn fn; };
    struct LoadHook { HookToken token; std::string key; LoadHookFn fn; };
    std::vector<SaveHook> save_hooks_;
    std::vector<LoadHook> load_hooks_;
    HookToken next_token_ = 1;

};

template <typename T> class DocumentT : public Document {
public:
    Model *model() override { return model_.get(); }
    // Data members
    std::unique_ptr<T> model_;
};

} // namespace augr