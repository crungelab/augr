#include <augr/core/document.h>

namespace augr {

// ---------- Hook registry ----------

Document::HookToken Document::AddSaveHook(SaveHookFn fn) {
    HookToken token = next_token_++;
    save_hooks_.push_back({token, std::move(fn)});
    return token;
}

Document::HookToken Document::AddLoadHook(LoadHookFn fn) {
    HookToken token = next_token_++;
    load_hooks_.push_back({token, std::move(fn)});
    return token;
}

void Document::RemoveSaveHook(HookToken token) {
    save_hooks_.erase(
        std::remove_if(save_hooks_.begin(), save_hooks_.end(),
                       [token](const SaveHook &h) { return h.token == token; }),
        save_hooks_.end());
}

void Document::RemoveLoadHook(HookToken token) {
    load_hooks_.erase(
        std::remove_if(load_hooks_.begin(), load_hooks_.end(),
                       [token](const LoadHook &h) { return h.token == token; }),
        load_hooks_.end());
}

} // namespace augr