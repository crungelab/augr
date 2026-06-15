#pragma once

#include "../widget/widget.h"

namespace augr {

class Frame : public Widget {
public:
    Frame(const std::string &label = "") : Widget(), label_(label) {}
    virtual ~Frame() = default;

    void Draw() override;
    void DrawMainMenuBar();
    virtual void OnBeforeDrawMainMenuBar();
    virtual void OnDrawMainMenuBar();

    virtual void Begin();
    virtual void End();

    // Accessors
    bool is_active();
    // Data members
    std::string label_;
    Vec2 window_position_ = {100.0f, 100.0f};
    Vec2 window_size_ = {320.0f, 240.0f};
    bool docked_ = false;
    bool window_pose_dirty_ = true;
};

} // namespace augr