// ui_builder.h
#pragma once

#include <memory>
#include <string>
#include <vector>

#include <augr/core/binding.h>
#include <augr/core/config.h>
#include <augr/core/model.h>

//#include <augr/core/ui/control/control.h>
//#include <augr/core/ui/control/control_meta.h>

namespace augr {

class ControlMeta;
class Parameter;

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

        // Non-copyable, movable
        BoxScope(const BoxScope &) = delete;
        BoxScope &operator=(const BoxScope &) = delete;
        BoxScope(BoxScope &&other) noexcept
            : builder(other.builder), closed_(other.closed_) {
            other.closed_ = true; // prevent double-close on move
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
    //UiBuilder() = default;
    UiBuilder(Model &model);

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
    // Sliders / knobs — return *this for chaining
    // -------------------------------------------------------------------------
    UiBuilder &Knob(const std::string &label, Parameter *param);

    UiBuilder &VSlider(const std::string &label, Parameter *param);

    UiBuilder &HSlider(const std::string &label, Parameter *param);

    UiBuilder &NumEntry(const std::string &label, Parameter *param);

    // -------------------------------------------------------------------------
    // Buttons — return *this for chaining
    // -------------------------------------------------------------------------
    UiBuilder &Button(const std::string &label, Parameter *param);

    UiBuilder &CheckButton(const std::string &label, Parameter *param);

    UiBuilder &ToggleButton(const std::string &label, Parameter *param);

    // -------------------------------------------------------------------------
    // Displays — return *this for chaining
    // -------------------------------------------------------------------------
    UiBuilder &NumDisplay(const std::string &label, const ControlMeta meta, ZoneBindingPtr binding,
                          int precision = 2);

    UiBuilder &TextDisplay(const std::string &label, const ControlMeta meta, ZoneBindingPtr binding,
                           char *names[], fy_real min, fy_real max);

    // -------------------------------------------------------------------------
    // Bargraphs — return *this for chaining
    // is_db: drives dB-scaled rendering; resolved by caller (e.g. FaustDspUi
    // checks its zones_ map before calling, hand-written modules pass directly)
    // -------------------------------------------------------------------------
    UiBuilder &HBarGraph(const std::string &label, Parameter *param);

    UiBuilder &VBarGraph(const std::string &label, Parameter *param);

    // -------------------------------------------------------------------------
    // Result
    // Returns the root Model* after building is complete.
    // Ownership remains with the UiBuilder — transfer or clone as needed.
    // Returns nullptr if no controls were added.
    // -------------------------------------------------------------------------
    Model *root() const { return root_; }

    // -------------------------------------------------------------------------
    // Module label
    // Typically set by FaustDspUi when it encounters the top-level box label.
    // Hand-written modules can set this directly or ignore it.
    // -------------------------------------------------------------------------
    const std::string &module_label() const { return module_label_; }
    void set_module_label(const std::string &label) { module_label_ = label; }

private:
    void PushModel(Model &model);
    Model *PopModel();
    void AddModel(Model &model);
    /*
    Parameter *MakeParameter(const std::string &label, float *zone,
                             fy_real init, fy_real min, fy_real max,
                             fy_real step);
    */
    std::vector<Model *> model_stack_;
    Model *root_ = nullptr;
    std::string module_label_;
};

} // namespace augr