#pragma once

#include <memory>
#include <string>
#include <vector>

#include <augr/binding.h>
#include <augr/config.h>
#include <augr/model.h>

namespace augr {

class ControlMeta;
class FloatParameter;
class EnumParameter;

class UiBuilder {
public:
    using ZoneBindingPtr = std::shared_ptr<BindingT<fy_real>>;

    // -------------------------------------------------------------------------
    // RAII box scope — returned by VBox/HBox/TabBox/FrameBox.
    // Calls CloseBox() on destruction, so nested layout works with auto _ = ...
    // For adapters that receive open/close as separate callbacks (e.g.
    // FaustDspUi), use the Open*/CloseBox() methods directly instead.
    // -------------------------------------------------------------------------
    struct BoxScope {
        UiBuilder &builder;

        explicit BoxScope(UiBuilder &b) : builder(b) {}

        BoxScope(const BoxScope &) = delete;
        BoxScope &operator=(const BoxScope &) = delete;
        BoxScope(BoxScope &&other) noexcept
            : builder(other.builder), closed_(other.closed_) {
            other.closed_ = true;
        }

        ~BoxScope() {
            if (!closed_)
                builder.CloseBox();
        }

    private:
        bool closed_ = false;
    };

    // -------------------------------------------------------------------------
    // Construction
    // -------------------------------------------------------------------------
    explicit UiBuilder(Model::Ptr model);

    // -------------------------------------------------------------------------
    // High-level RAII layout — preferred for hand-written modules
    // -------------------------------------------------------------------------
    BoxScope VBox(const std::string &label);
    BoxScope HBox(const std::string &label);
    BoxScope TabBox(const std::string &label);
    BoxScope FrameBox(const std::string &label);

    // -------------------------------------------------------------------------
    // Low-level layout — for adapters (e.g. FaustDspUi) where open/close
    // arrive as separate callbacks and RAII scoping is not possible.
    // -------------------------------------------------------------------------
    void OpenVBox(const std::string &label);
    void OpenHBox(const std::string &label);
    void OpenTabBox(const std::string &label);
    void OpenFrameBox(const std::string &label);
    void CloseBox();

    // -------------------------------------------------------------------------
    // Sliders / knobs
    // -------------------------------------------------------------------------
    UiBuilder &Knob(const std::string &label, FloatParameter *param);
    UiBuilder &VSlider(const std::string &label, FloatParameter *param);
    UiBuilder &HSlider(const std::string &label, FloatParameter *param);
    UiBuilder &NumEntry(const std::string &label, FloatParameter *param);

    // -------------------------------------------------------------------------
    // Buttons
    // -------------------------------------------------------------------------
    UiBuilder &Button(const std::string &label, FloatParameter *param);
    UiBuilder &CheckButton(const std::string &label, FloatParameter *param);
    UiBuilder &ToggleButton(const std::string &label, FloatParameter *param);

    // -------------------------------------------------------------------------
    // Dropdowns / combos
    // -------------------------------------------------------------------------
    UiBuilder &Combo(const std::string &label, EnumParameter *param);

    // -------------------------------------------------------------------------
    // Displays
    // -------------------------------------------------------------------------
    UiBuilder &NumDisplay(const std::string &label, const ControlMeta meta,
                          ZoneBindingPtr binding, int precision = 2);
    UiBuilder &TextDisplay(const std::string &label, const ControlMeta meta,
                           ZoneBindingPtr binding, char *names[], fy_real min,
                           fy_real max);

    // -------------------------------------------------------------------------
    // Bargraphs
    // -------------------------------------------------------------------------
    UiBuilder &HBarGraph(const std::string &label, FloatParameter *param);
    UiBuilder &VBarGraph(const std::string &label, FloatParameter *param);

    // -------------------------------------------------------------------------
    // Result
    // Returns the root model after building is complete. May be null if no
    // controls were added.
    // -------------------------------------------------------------------------
    Model::Ptr root() const {
        return model_stack_.empty() ? nullptr : model_stack_.front();
    }

    // -------------------------------------------------------------------------
    // Module label
    // -------------------------------------------------------------------------
    const std::string &module_label() const { return module_label_; }
    void set_module_label(const std::string &label) { module_label_ = label; }

private:
    void PushModel(Model::Ptr model);
    Model::Ptr PopModel();

    std::vector<Model::Ptr> model_stack_;
    std::string module_label_;
};

} // namespace augr