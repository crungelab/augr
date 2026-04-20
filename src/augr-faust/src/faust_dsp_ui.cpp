#include <augr/rack/module/module.h>

#include <augr/faust/faust_dsp.h>
#include <augr/faust/faust_dsp_ui.h>

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

FaustDspUi::FaustDspUi(FaustDsp &m) : UI(), m_(&m) { PushModel(m); }

void FaustDspUi::PushModel(Model &model) { model_stack_.push_back(&model); }

Model *FaustDspUi::PopModel() {
    Model *top = model_stack_.back();
    model_stack_.pop_back();
    return top;
}

void FaustDspUi::AddModel(Model &model) {
    Model &top = *model_stack_.back();
    top.AddChild(model);
}

void FaustDspUi::declare(float *zone, const char *key, const char *value) {
    zones_[zone].declare(key, value);
}

// ---------------------------------------------------------------------------
// Helper: build a Parameter, register it with FaustDsp, return a
// FloatParameterControl view.  The Parameter is owned by FaustDsp; the
// FloatParameterControl is owned by the model tree.
// ---------------------------------------------------------------------------

FloatParameter* FaustDspUi::MakeParameter(const char *label, float *zone,
                                     const fy_real init, const fy_real min,
                                     const fy_real max, const fy_real step) {

    auto meta = std::move(zones_[zone]);
    auto binding = MakePointerBinding(zone);
    auto param = FloatParameter::Make(label, std::move(meta), std::move(binding),
                                 init, min, max, step);
    FloatParameter *raw = param.get();
    m_->AddParameter(std::move(param)); // FaustDsp owns the Parameter
    return raw;
}

// ---------------------------------------------------------------------------
// Buttons / toggles — no range, map to [0, 1] boolean parameters
// ---------------------------------------------------------------------------

void FaustDspUi::addButton(const char *label, float *zone) {
    AddModel(*new Button(label, MakeParameter(label, zone, 0, 0, 1, 1)));
}

void FaustDspUi::addToggleButton(const char *label, float *zone) {
    AddModel(*new ToggleButton(label, MakeParameter(label, zone, 0, 0, 1, 1)));
}

void FaustDspUi::addCheckButton(const char *label, float *zone) {
    AddModel(*new CheckButton(label, MakeParameter(label, zone, 0, 0, 1, 1)));
}

// ---------------------------------------------------------------------------
// Sliders / knobs / num entry — full range
// ---------------------------------------------------------------------------

void FaustDspUi::addVerticalSlider(const char *label, float *zone, float init,
                                   float min, float max, float step) {
    if (zones_[zone].IsKnob())
        return addKnob(label, zone, init, min, max, step);
    AddModel(*new VSlider(label, MakeParameter(label, zone, init, min, max, step)));
}

void FaustDspUi::addHorizontalSlider(const char *label, float *zone, float init,
                                     float min, float max, float step) {
    if (zones_[zone].IsKnob())
        return addKnob(label, zone, init, min, max, step);
    AddModel(*new HSlider(label, MakeParameter(label, zone, init, min, max, step)));
}

void FaustDspUi::addKnob(const char *label, float *zone, float init, float min,
                         float max, float step) {
    AddModel(*new Knob(label, MakeParameter(label, zone, init, min, max, step)));
}

void FaustDspUi::addNumEntry(const char *label, float *zone, float init,
                             float min, float max, float step) {
    AddModel(*new NumEntry(label, MakeParameter(label, zone, init, min, max, step)));
}

// ---------------------------------------------------------------------------
// Bargraphs — read-only, no step
// ---------------------------------------------------------------------------

void FaustDspUi::addHorizontalBargraph(const char *label, float *zone,
                                       float min, float max) {
    AddModel(*new HBarGraph(label, MakeParameter(label, zone, min, min, max, 0)));
}

void FaustDspUi::addVerticalBargraph(const char *label, float *zone, float min,
                                     float max) {
    AddModel(*new VBarGraph(label, MakeParameter(label, zone, min, min, max, 0)));
}

// ---------------------------------------------------------------------------
// Displays
// ---------------------------------------------------------------------------

void FaustDspUi::addNumDisplay(const char *label, float *zone,
                               const int precision) {
    const auto meta = zones_[zone];
    AddModel(*new NumDisplay(label, meta, MakePointerBinding(zone), precision));
}

void FaustDspUi::addTextDisplay(const char *label, float *zone, char *names[],
                                const float min, const float max) {

    const auto meta = zones_[zone];
    AddModel(*new TextDisplay(label, meta, MakePointerBinding(zone), names, min, max));
}

// ---------------------------------------------------------------------------
// Box layout
// ---------------------------------------------------------------------------

void FaustDspUi::openFrameBox(const char *label) {
    PushModel(*new FrameBox(label));
}

void FaustDspUi::openTabBox(const char *label) {
    PushModel(*new TabBox(label));
}

void FaustDspUi::openHorizontalBox(const char *label) {
    auto *box = new HBox(label);
    if (is_top_) {
        is_top_ = false;
        m_->label_ = label;
        box->is_top_level_ = true;
    }
    if (strcmp(label, "0x00") == 0)
        box->label_visible_ = false;
    PushModel(*box);
}

void FaustDspUi::openVerticalBox(const char *label) {
    auto *box = new VBox(label);
    if (is_top_) {
        is_top_ = false;
        m_->label_ = label;
        box->is_top_level_ = true;
    }
    if (strcmp(label, "0x00") == 0)
        box->label_visible_ = false;
    PushModel(*box);
}

void FaustDspUi::closeBox() {
    Model *top = PopModel();
    if (model_stack_.empty())
        return;
    AddModel(*top);
}

} // namespace augr