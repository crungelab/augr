#pragma once

#include <functional>
#include <mutex>
#include <vector>

#include <augr/rack/subrack.h>
#include <augr/rack/rack_config.h>

namespace augr {

class Rack : public Subrack {
public:
    Rack() { singleton_ = this; }
    virtual ~Rack();

    // Top-level convenience: creates the standard set of boundary devices
    // based on config_. Delegates to Subrack's Create*Device helpers.
    void CreateDefaultDevices();

    // -- Action queue --------------------------------------------------
    // Thread bridge: any thread enqueues, audio thread drains via
    // ProcessActions() / ProcessUpdateActions().
    void EnqueueAction(std::function<void()> action,
                       std::function<void()> update_action = nullptr);
    void EnqueueUpdateAction(std::function<void()> action);

    void ProcessActions();
    void ProcessUpdateActions();

    // -- Lifecycle -----------------------------------------------------
    virtual bool Start() {
        running_ = true;
        return true;
    }
    virtual void Stop() { running_ = false; }

    // -- Accessors -----------------------------------------------------
    static Rack &singleton() { return *singleton_; }
    bool IsRunning() const { return running_; }

    // Data members
    static Rack *singleton_;
    RackConfig config_;

    std::mutex mutex_;
    std::vector<std::function<void()>> pending_actions_;
    std::vector<std::function<void()>> pending_update_actions_;

    REFLECT_ENABLE(Subrack)

private:
    bool running_ = false;
};

} // namespace augr