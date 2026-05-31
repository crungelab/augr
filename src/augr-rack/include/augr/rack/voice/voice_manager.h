#pragma once

#include <string>
#include <vector>
#include <map>

namespace augr {

class Voice;

class VoiceManager {
public:
    // Accessors
    static VoiceManager &singleton() { return *singleton_; }

    // Generates a name like "Voice", "Voice 2", "Voice 3" ... that
    // doesn't collide with anything currently registered.
    std::string AllocateUniqueName(const std::string &base) {
        if (voices_.find(base) == voices_.end()) return base;
        for (int i = 2; ; ++i) {
            std::string candidate = base + " " + std::to_string(i);
            if (voices_.find(candidate) == voices_.end()) return candidate;
        }
    }

    // Rename a voice. Returns true on success, false on collision.
    bool RenameVoice(const std::string &old_name,
                     const std::string &new_name) {
        if (voices_.count(new_name)) return false;
        auto it = voices_.find(old_name);
        if (it == voices_.end()) return false;
        Voice *v = it->second;
        voices_.erase(it);
        voices_[new_name] = v;
        return true;
    }

    // Enumerate names (for the voicebank's dropdown).
    std::vector<std::string> Names() const {
        std::vector<std::string> out;
        out.reserve(voices_.size());
        for (const auto &kv : voices_) out.push_back(kv.first);
        return out;
    }

    void AddVoice(const std::string &name, Voice *voice) {
        voices_[name] = voice;
    }
    Voice *GetVoice(const std::string &name) {
        auto it = voices_.find(name);
        if (it != voices_.end()) {
            return it->second;
        }
        return nullptr;
    }
    void RemoveVoice(const std::string &name) {
        voices_.erase(name);
    }
    // Data members
    static VoiceManager *singleton_;
    std::map<std::string, Voice *> voices_;
};

} // namespace augr