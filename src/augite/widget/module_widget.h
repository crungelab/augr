#pragma once

#include <string>

#include <imgui.h>
#include <imnodes.h>

#include <augr/core/math/vec2.h>
#include <augr/rack/module/module.h>

#include "augr/rack/rack_doc.h"
#include "widget.h"

namespace augr {

class Document;
class Frame;

class ModuleWidget : public ModelWidget {
public:
    explicit ModuleWidget(Module &model) : ModelWidget(model) {
        window_name_ = std::string(model.label_) + "###module_" +
                       std::to_string(model.id_);
    }
    void ShowWindow() { is_open_ = true; }
    void HideWindow() { is_open_ = false; }

    void Draw() override;
    void DrawNode();
    virtual void DrawViewer();

    virtual void DrawNodeContent() {}
    virtual void DrawView() { this->DrawChildren(); }

    // Event handlers
    virtual void OnLeftDoubleClick(RackDoc &doc, Frame &parent_frame) {
        is_open_ = !is_open_;
        window_pose_dirty_ = true;
    }

    // Accessors
    Module &model() { return *static_cast<Module *>(model_); }
    const Module &model() const { return *static_cast<const Module *>(model_); }

    // Conversion helpers — keep Vec2 (ImGui-free) at the data layer
    // and only touch ImVec2 at the draw boundary.
    static ImVec2 ToImVec2(const Vec2 &v) { return ImVec2(v.x, v.y); }
    static Vec2 FromImVec2(const ImVec2 &v) { return Vec2{v.x, v.y}; }

    // Data members
    bool is_open_ = false;
    std::string window_name_;
    //
    Vec2 grid_position_ = {0.0f, 0.0f};
    Vec2 window_position_ = {100.0f, 100.0f};
    Vec2 window_size_ = {320.0f, 240.0f};

    // Set to true when state has been loaded from JSON or explicitly
    // assigned (e.g. catalog spawn), telling Draw to push the values
    // into ImNodes/ImGui on the next frame. Cleared after the push.
    bool position_dirty_ = true;
    bool window_pose_dirty_ = true;
};

template <typename T>
class ModuleWidgetT : public ModelWidgetT<T, ModuleWidget> {
public:
    explicit ModuleWidgetT(T &model) : ModelWidgetT<T, ModuleWidget>(model) {}
};

using DefaultModuleWidget = ModuleWidgetT<Module>;

} // namespace augr