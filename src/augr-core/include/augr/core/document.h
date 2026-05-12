#pragma once

#include <filesystem>
#include <optional>
#include <string>
#include <vector>

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

protected:
    void SetPath(std::filesystem::path p) { path_ = std::move(p); }
    void ClearPath() { path_.reset(); }
    void MarkClean() { modified_ = false; }

private:
    std::optional<std::filesystem::path> path_;
    bool modified_ = false;
};

template <typename T> class DocumentT : public Document {
public:
    Model *model() override { return model_.get(); }
    // Data members
    std::unique_ptr<T> model_;
};

} // namespace augr