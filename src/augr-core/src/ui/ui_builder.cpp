#include <augr/core/ui/ui_builder.h>

#include <augr/core/ui/control/button.h>
#include <augr/core/ui/control/check_button.h>
#include <augr/core/ui/control/combo.h>
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

UiBuilder::UiBuilder(Model::Ptr model) {
    model_stack_.push_back(std::move(model));
}

void UiBuilder::PushModel(Model::Ptr model) {
    model_stack_.push_back(std::move(model));
}

Model::Ptr UiBuilder::PopModel() {
    auto top = std::move(model_stack_.back());
    model_stack_.pop_back();
    return top;
}

// ---------------------------------------------------------------------------
// Buttons / toggles
// ---------------------------------------------------------------------------

UiBuilder &UiBuilder::Button(const std::string &label, FloatParameter *param) {
    Model::MakeFresh<augr::Button>(model_stack_.back(), label, param);
    return *this;
}

UiBuilder &UiBuilder::ToggleButton(const std::string &label,
                                   FloatParameter *param) {
    Model::MakeFresh<augr::ToggleButton>(model_stack_.back(), label, param);
    return *this;
}

UiBuilder &UiBuilder::CheckButton(const std::string &label,
                                  FloatParameter *param) {
    Model::MakeFresh<augr::CheckButton>(model_stack_.back(), label, param);
    return *this;
}

// ---------------------------------------------------------------------------
// Dropdowns
// ---------------------------------------------------------------------------

UiBuilder &UiBuilder::Combo(const std::string &label, EnumParameter *param) {
    Model::MakeFresh<augr::Combo>(model_stack_.back(), label, param);
    return *this;
}

// ---------------------------------------------------------------------------
// Sliders / knobs / num entry
// ---------------------------------------------------------------------------

UiBuilder &UiBuilder::VSlider(const std::string &label, FloatParameter *param) {
    Model::MakeFresh<augr::VSlider>(model_stack_.back(), label, param);
    return *this;
}

UiBuilder &UiBuilder::HSlider(const std::string &label, FloatParameter *param) {
    Model::MakeFresh<augr::HSlider>(model_stack_.back(), label, param);
    return *this;
}

UiBuilder &UiBuilder::Knob(const std::string &label, FloatParameter *param) {
    Model::MakeFresh<augr::Knob>(model_stack_.back(), label, param);
    return *this;
}

UiBuilder &UiBuilder::NumEntry(const std::string &label,
                               FloatParameter *param) {
    Model::MakeFresh<augr::NumEntry>(model_stack_.back(), label, param);
    return *this;
}

// ---------------------------------------------------------------------------
// Bargraphs
// ---------------------------------------------------------------------------

UiBuilder &UiBuilder::HBarGraph(const std::string &label,
                                FloatParameter *param) {
    Model::MakeFresh<augr::HBarGraph>(model_stack_.back(), label, param);
    return *this;
}

UiBuilder &UiBuilder::VBarGraph(const std::string &label,
                                FloatParameter *param) {
    Model::MakeFresh<augr::VBarGraph>(model_stack_.back(), label, param);
    return *this;
}

// ---------------------------------------------------------------------------
// Displays
// ---------------------------------------------------------------------------

UiBuilder &UiBuilder::NumDisplay(const std::string &label,
                                 const ControlMeta meta, ZoneBindingPtr binding,
                                 const int precision) {
    Model::MakeFresh<augr::NumDisplay>(model_stack_.back(), label, meta, binding, precision);
    return *this;
}

UiBuilder &UiBuilder::TextDisplay(const std::string &label,
                                  const ControlMeta meta,
                                  ZoneBindingPtr binding, char *names[],
                                  const float min, const float max) {
    Model::MakeFresh<augr::TextDisplay>(model_stack_.back(), label, meta, binding, names, min, max);
    return *this;
}

// ---------------------------------------------------------------------------
// Box layout
// ---------------------------------------------------------------------------

UiBuilder::BoxScope UiBuilder::VBox(const std::string &label) {
    OpenVBox(label);
    return BoxScope(*this);
}

UiBuilder::BoxScope UiBuilder::HBox(const std::string &label) {
    OpenHBox(label);
    return BoxScope(*this);
}

UiBuilder::BoxScope UiBuilder::TabBox(const std::string &label) {
    OpenTabBox(label);
    return BoxScope(*this);
}

UiBuilder::BoxScope UiBuilder::FrameBox(const std::string &label) {
    OpenFrameBox(label);
    return BoxScope(*this);
}

void UiBuilder::OpenFrameBox(const std::string &label) {
    PushModel(std::make_shared<augr::FrameBox>(label));
}

void UiBuilder::OpenTabBox(const std::string &label) {
    PushModel(std::make_shared<augr::TabBox>(label));
}

void UiBuilder::OpenHBox(const std::string &label) {
    auto box = std::make_shared<augr::HBox>(label);
    if (model_stack_.empty())
        box->is_top_level_ = true;
    if (strcmp(label.c_str(), "0x00") == 0)
        box->label_visible_ = false;
    PushModel(std::move(box));
}

void UiBuilder::OpenVBox(const std::string &label) {
    auto box = std::make_shared<augr::VBox>(label);
    if (model_stack_.empty())
        box->is_top_level_ = true;
    if (strcmp(label.c_str(), "0x00") == 0)
        box->label_visible_ = false;
    PushModel(std::move(box));
}

void UiBuilder::CloseBox() {
    auto top = PopModel();
    if (model_stack_.empty())
        return;
    model_stack_.back()->AddChild(std::move(top));
}

} // namespace augr