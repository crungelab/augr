#include <augr/rack/module/module.h>

#include <augr/faust/faust_dsp.h>
#include <augr/faust/faust_dsp_ui.h>

#include <augr/core/control/button.h>
#include <augr/core/control/check_button.h>
#include <augr/core/control/frame_box.h>
#include <augr/core/control/h_bargraph.h>
#include <augr/core/control/h_box.h>
#include <augr/core/control/h_slider.h>
#include <augr/core/control/knob.h>
#include <augr/core/control/num_display.h>
#include <augr/core/control/num_entry.h>
#include <augr/core/control/tab_box.h>
#include <augr/core/control/text_display.h>
#include <augr/core/control/toggle_button.h>
#include <augr/core/control/v_bargraph.h>
#include <augr/core/control/v_box.h>
#include <augr/core/control/v_slider.h>

namespace augr {

std::shared_ptr<BindingT<fy_real>>
MakeZoneBinding(float *zone) {
    return std::make_shared<ZoneBinding>(zone);
}

FaustDspUi::FaustDspUi(FaustDsp &m) : UI(), m_(&m) { PushModel(m); }

void FaustDspUi::PushModel(Model &model) { models_.push_back(&model); }

Model *FaustDspUi::PopModel() {
    Model *top = models_.back();
    models_.pop_back();
    return top;
}

void FaustDspUi::AddModel(Model &model) {
    Model &top = *models_.back();
    top.AddChild(model);
}

void FaustDspUi::declare(float *zone, const char *key, const char *value) {
    // Always store into the zone entry, creating it if needed — the original
    // code created an empty Zone() and discarded key/value on first sight.
    zones_[zone].declare(key, value);
}

// Helper: look up the Zone for a pointer and check whether "unit" == "dB".
static bool ZoneIsDb(const map<FAUSTFLOAT *, Zone> &zones, float *zone) {
    auto it = zones.find(zone);
    if (it == zones.end())
        return false;
    const char *unit = it->second.get("unit");
    return unit && (strcmp(unit, "dB") == 0 || strcmp(unit, "db") == 0);
}

void FaustDspUi::addButton(const char *label, float *zone) {
    AddModel(*new Button(label, MakeZoneBinding(zone)));
}

void FaustDspUi::addToggleButton(const char *label, float *zone) {
    AddModel(*new ToggleButton(label, MakeZoneBinding(zone)));
}

void FaustDspUi::addCheckButton(const char *label, float *zone) {
    AddModel(*new CheckButton(label, MakeZoneBinding(zone)));
}

void FaustDspUi::addVerticalSlider(const char *label, float *zone, float init,
                                   float min, float max, float step) {
    if (zones_[zone].isKnob())
        return addKnob(label, zone, init, min, max, step);
    AddModel(*new VSlider(label, MakeZoneBinding(zone), init, min, max, step));
}

void FaustDspUi::addHorizontalSlider(const char *label, float *zone, float init,
                                     float min, float max, float step) {
    if (zones_[zone].isKnob())
        return addKnob(label, zone, init, min, max, step);
    AddModel(*new HSlider(label, MakeZoneBinding(zone), init, min, max, step));
}

void FaustDspUi::addKnob(const char *label, float *zone, float init, float min,
                         float max, float step) {
    AddModel(*new Knob(label, MakeZoneBinding(zone), init, min, max, step));
}

void FaustDspUi::addNumEntry(const char *label, float *zone, float init,
                             float min, float max, float step) {
    AddModel(*new NumEntry(label, MakeZoneBinding(zone), init, min, max, step));
}

void FaustDspUi::addNumDisplay(const char *label, float *zone, int precision) {
    AddModel(*new NumDisplay(label, MakeZoneBinding(zone), precision));
}

void FaustDspUi::addTextDisplay(const char *label, float *zone, char *names[],
                                float min, float max) {
    AddModel(*new TextDisplay(label, MakeZoneBinding(zone), names, min, max));
}

void FaustDspUi::addHorizontalBargraph(const char *label, float *zone,
                                       float min, float max) {
    auto *bg = new HBarGraph(label, MakeZoneBinding(zone), min, max);
    bg->is_db_ = ZoneIsDb(zones_, zone);
    AddModel(*bg);
}

void FaustDspUi::addVerticalBargraph(const char *label, float *zone, float min,
                                     float max) {
    auto *bg = new VBarGraph(label, MakeZoneBinding(zone), min, max);
    bg->is_db_ = ZoneIsDb(zones_, zone);
    AddModel(*bg);
}

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
    if (models_.empty())
        return;
    AddModel(*top);
}

} // namespace augr