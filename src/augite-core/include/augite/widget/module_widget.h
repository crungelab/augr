#pragma once

#include <string>

#include <imnodes.h>

#include <augr/rack/module/module.h>

#include "augr/rack/rack_doc.h"
#include <augite/widget/widget.h>

namespace augr {

class Document;
class Frame;

class ModuleWidget : public ModelWidget {
public:
    explicit ModuleWidget(Module &model) : ModelWidget(model) {}

    void Draw() override;
    void DrawNode();

    virtual void DrawNodeContent() {}

    // Accessors
    Module &model() { return *static_cast<Module *>(model_); }
    const Module &model() const { return *static_cast<const Module *>(model_); }

    // Data members
    Vec2 grid_position_ = {0.0f, 0.0f};

    // Set to true when state has been loaded from JSON or explicitly
    // assigned (e.g. catalog spawn), telling Draw to push the values
    // into ImNodes/ImGui on the next frame. Cleared after the push.
    bool position_dirty_ = true;
};

template <typename T>
class ModuleWidgetT : public ModelWidgetT<T, ModuleWidget> {
public:
    explicit ModuleWidgetT(T &model) : ModelWidgetT<T, ModuleWidget>(model) {}
};

} // namespace augr