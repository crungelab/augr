#include <augr/core/ui/ui_builder.h>

#include <augr/core/ui/control/button.h>
#include <augr/core/ui/control/check_button.h>
#include <augr/core/ui/control/frame_box.h>
#include <augr/core/ui/control/h_bargraph.h>
#include <augr/core/ui/control/h_box.h>
#include <augr/core/ui/control/h_slider.h>
#include <augr/core/ui/control/knob.h>
#include <augr/core/ui/control/num_display.h>
#include <augr/core/ui/control/num_entry.h>
#include <augr/core/ui/control/tab_box.h>
#include <augr/core/ui/control/text_display.h>
#include <augr/core/ui/control/toggle_button.h>
#include <augr/core/ui/control/v_bargraph.h>
#include <augr/core/ui/control/v_box.h>
#include <augr/core/ui/control/v_slider.h>

namespace augr {

std::unique_ptr<BindingT<fy_real>> MakeZoneBinding(float *zone) {
    return std::make_unique<ZoneBinding>(zone);
}

// UiBuilder::UiBuilder(FaustDsp &m) : UI(), m_(&m) { PushModel(m); }

void UiBuilder::PushModel(Model &model) { model_stack_.push_back(&model); }

Model *UiBuilder::PopModel() {
    Model *top = model_stack_.back();
    model_stack_.pop_back();
    return top;
}

void UiBuilder::AddModel(Model &model) {
    Model &top = *model_stack_.back();
    top.AddChild(model);
}

/*
void UiBuilder::declare(float *zone, const char *key, const char *value) {
    zones_[zone].declare(key, value);
}
*/

// ---------------------------------------------------------------------------
// Helper: build a Parameter, register it with FaustDsp, return a
// ParameterControl view.  The Parameter is owned by FaustDsp; the
// ParameterControl is owned by the model tree.
// ---------------------------------------------------------------------------

/*
Parameter* UiBuilder::MakeParameter(const std::string& label, float *zone,
                                     const fy_real init, const fy_real min,
                                     const fy_real max, const fy_real step) {

    auto meta = std::move(zones_[zone]);
    auto binding = MakeZoneBinding(zone);
    auto param = Parameter::Make(label, std::move(meta), std::move(binding),
                                 init, min, max, step);
    Parameter *raw = param.get();
    m_->AddParameter(std::move(param)); // FaustDsp owns the Parameter
    return raw;
}
*/

// ---------------------------------------------------------------------------
// Buttons / toggles — no range, map to [0, 1] boolean parameters
// ---------------------------------------------------------------------------

UiBuilder &UiBuilder::Button(const std::string &label, Parameter *param) {
    AddModel(*new augr::Button(label, param));
    return *this;
}

UiBuilder &UiBuilder::ToggleButton(const std::string &label, Parameter *param) {
    AddModel(*new augr::ToggleButton(label, param));
    return *this;
}

UiBuilder &UiBuilder::CheckButton(const std::string &label, Parameter *param) {
    AddModel(*new augr::CheckButton(label, param));
    return *this;
}

// ---------------------------------------------------------------------------
// Sliders / knobs / num entry — full range
// ---------------------------------------------------------------------------

UiBuilder &UiBuilder::VSlider(const std::string &label, Parameter *param) {
    AddModel(*new augr::VSlider(label, param));
    return *this;
}

UiBuilder &UiBuilder::HSlider(const std::string &label, Parameter *param) {
    AddModel(*new augr::HSlider(label, param));
    return *this;
}

UiBuilder &UiBuilder::Knob(const std::string &label, Parameter *param) {
    AddModel(*new augr::Knob(label, param));
    return *this;
}

UiBuilder &UiBuilder::NumEntry(const std::string &label, Parameter *param) {
    AddModel(*new augr::NumEntry(label, param));
    return *this;
}

// ---------------------------------------------------------------------------
// Bargraphs — read-only, no step
// ---------------------------------------------------------------------------

UiBuilder &UiBuilder::HBarGraph(const std::string &label, Parameter *param) {
    AddModel(*new augr::HBarGraph(label, param));
    return *this;
}

UiBuilder &UiBuilder::VBarGraph(const std::string &label, Parameter *param) {
    AddModel(*new augr::VBarGraph(label, param));
    return *this;
}

// ---------------------------------------------------------------------------
// Displays
// ---------------------------------------------------------------------------

UiBuilder &UiBuilder::NumDisplay(const std::string &label,
                                 const ControlMeta meta, ZoneBindingPtr binding,
                                 const int precision) {
    AddModel(*new augr::NumDisplay(label, meta, binding, precision));
    return *this;
}

UiBuilder &UiBuilder::TextDisplay(const std::string &label,
                                  const ControlMeta meta,
                                  ZoneBindingPtr binding, char *names[],
                                  const float min, const float max) {

    AddModel(*new augr::TextDisplay(label, meta, binding, names, min, max));
    return *this;
}

// ---------------------------------------------------------------------------
// Box layout
// ---------------------------------------------------------------------------

void UiBuilder::OpenFrameBox(const std::string &label) {
    PushModel(*new augr::FrameBox(label));
}

void UiBuilder::OpenTabBox(const std::string &label) {
    PushModel(*new augr::TabBox(label));
}

void UiBuilder::OpenHBox(const std::string &label) {
    auto *box = new augr::HBox(label);
    if (model_stack_.empty())
        box->is_top_level_ = true;
    if (strcmp(label.c_str(), "0x00") == 0)
        box->label_visible_ = false;
    PushModel(*box);
}

void UiBuilder::OpenVBox(const std::string &label) {
    auto *box = new augr::VBox(label);
    if (model_stack_.empty())
        box->is_top_level_ = true;
    if (strcmp(label.c_str(), "0x00") == 0)
        box->label_visible_ = false;
    PushModel(*box);
}

void UiBuilder::CloseBox() {
    Model *top = PopModel();
    if (model_stack_.empty())
        return;
    AddModel(*top);
}

} // namespace augr