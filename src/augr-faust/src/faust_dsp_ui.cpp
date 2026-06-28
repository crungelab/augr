#include <augr/rack/module/module.h>

#include <augr/faust/faust_dsp.h>
#include <augr/faust/faust_dsp_ui.h>

#include <augr/ui/control/button.h>
#include <augr/ui/control/check_button.h>
#include <augr/ui/control/frame_box.h>
#include <augr/ui/control/h_bargraph.h>
#include <augr/ui/control/h_box.h>
#include <augr/ui/control/h_slider.h>
#include <augr/ui/control/knob.h>
#include <augr/ui/control/num_display.h>
#include <augr/ui/control/num_entry.h>
#include <augr/ui/control/tab_box.h>
#include <augr/ui/control/text_display.h>
#include <augr/ui/control/toggle_button.h>
#include <augr/ui/control/v_bargraph.h>
#include <augr/ui/control/v_box.h>
#include <augr/ui/control/v_slider.h>

namespace augr {

FaustDspUi::FaustDspUi(FaustDsp &m) : UI(), m_(&m) {
    model_stack_.push_back(m.console_);
}

void FaustDspUi::PushModel(Model::Ptr model) {
    model_stack_.push_back(std::move(model));
}

Model::Ptr FaustDspUi::PopModel() {
    auto top = std::move(model_stack_.back());
    model_stack_.pop_back();
    return top;
}

void FaustDspUi::declare(float *zone, const char *key, const char *value) {
    zones_[zone].declare(key, value);
}

// ---------------------------------------------------------------------------
// Helper: build a Parameter, register it with FaustDsp, return a raw
// observer pointer. The Parameter is owned by FaustDsp; controls are
// owned by the model tree.
// ---------------------------------------------------------------------------

FloatParameter *FaustDspUi::MakeParameter(const char *label, float *zone,
                                          const fy_real init, const fy_real min,
                                          const fy_real max,
                                          const fy_real step) {
    auto meta = std::move(zones_[zone]);
    auto binding = MakePointerBinding(zone);
    auto param = FloatParameter::Make(label, std::move(meta),
                                      std::move(binding), init, min, max, step);
    FloatParameter *raw = param.get();
    m_->AddParameter(std::move(param));
    return raw;
}

// ---------------------------------------------------------------------------
// Buttons / toggles
// ---------------------------------------------------------------------------

void FaustDspUi::addButton(const char *label, float *zone) {
    Model::MakeFresh<Button>(model_stack_.back(), label,
                             MakeParameter(label, zone, 0, 0, 1, 1));
}

void FaustDspUi::addToggleButton(const char *label, float *zone) {
    Model::MakeFresh<ToggleButton>(model_stack_.back(), label,
                                   MakeParameter(label, zone, 0, 0, 1, 1));
}

void FaustDspUi::addCheckButton(const char *label, float *zone) {
    Model::MakeFresh<CheckButton>(model_stack_.back(), label,
                                  MakeParameter(label, zone, 0, 0, 1, 1));
}

// ---------------------------------------------------------------------------
// Sliders / knobs / num entry
// ---------------------------------------------------------------------------

void FaustDspUi::addVerticalSlider(const char *label, float *zone, float init,
                                   float min, float max, float step) {
    if (zones_[zone].IsKnob())
        return addKnob(label, zone, init, min, max, step);
    Model::MakeFresh<VSlider>(model_stack_.back(), label,
                              MakeParameter(label, zone, init, min, max, step));
}

void FaustDspUi::addHorizontalSlider(const char *label, float *zone, float init,
                                     float min, float max, float step) {
    if (zones_[zone].IsKnob())
        return addKnob(label, zone, init, min, max, step);
    Model::MakeFresh<HSlider>(model_stack_.back(), label,
                              MakeParameter(label, zone, init, min, max, step));
}

void FaustDspUi::addKnob(const char *label, float *zone, float init, float min,
                         float max, float step) {
    Model::MakeFresh<Knob>(model_stack_.back(), label,
                           MakeParameter(label, zone, init, min, max, step));
}

void FaustDspUi::addNumEntry(const char *label, float *zone, float init,
                             float min, float max, float step) {
    Model::MakeFresh<NumEntry>(
        model_stack_.back(), label,
        MakeParameter(label, zone, init, min, max, step));
}

// ---------------------------------------------------------------------------
// Bargraphs
// ---------------------------------------------------------------------------

void FaustDspUi::addHorizontalBargraph(const char *label, float *zone,
                                       float min, float max) {
    Model::MakeFresh<HBarGraph>(model_stack_.back(), label,
                                MakeParameter(label, zone, min, min, max, 0));
}

void FaustDspUi::addVerticalBargraph(const char *label, float *zone, float min,
                                     float max) {
    Model::MakeFresh<VBarGraph>(model_stack_.back(), label,
                                MakeParameter(label, zone, min, min, max, 0));
}

// ---------------------------------------------------------------------------
// Displays
// ---------------------------------------------------------------------------

void FaustDspUi::addNumDisplay(const char *label, float *zone,
                               const int precision) {
    const auto meta = zones_[zone];
    Model::MakeFresh<NumDisplay>(model_stack_.back(), label, meta,
                                 MakePointerBinding(zone), precision);
}

void FaustDspUi::addTextDisplay(const char *label, float *zone, char *names[],
                                const float min, const float max) {
    const auto meta = zones_[zone];
    Model::MakeFresh<TextDisplay>(model_stack_.back(), label, meta,
                                  MakePointerBinding(zone), names, min, max);
}

// ---------------------------------------------------------------------------
// Box layout
// ---------------------------------------------------------------------------

void FaustDspUi::openFrameBox(const char *label) {
    PushModel(std::make_shared<FrameBox>(label));
}

void FaustDspUi::openTabBox(const char *label) {
    PushModel(std::make_shared<TabBox>(label));
}

void FaustDspUi::openHorizontalBox(const char *label) {
    auto box = std::make_shared<HBox>(label);
    if (is_top_) {
        is_top_ = false;
        m_->label_ = label;
        box->is_top_level_ = true;
    }
    if (strcmp(label, "0x00") == 0)
        box->label_visible_ = false;
    PushModel(std::move(box));
}

void FaustDspUi::openVerticalBox(const char *label) {
    auto box = std::make_shared<VBox>(label);
    if (is_top_) {
        is_top_ = false;
        m_->label_ = label;
        box->is_top_level_ = true;
    }
    if (strcmp(label, "0x00") == 0)
        box->label_visible_ = false;
    PushModel(std::move(box));
}

void FaustDspUi::closeBox() {
    auto top = PopModel();
    if (model_stack_.empty())
        return;
    model_stack_.back()->AddChild(std::move(top));
}

} // namespace augr