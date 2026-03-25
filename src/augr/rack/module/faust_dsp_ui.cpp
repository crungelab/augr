#include <augr/rack/module/module.h>

#include <augr/rack/module/faust_dsp.h>
#include <augr/rack/module/faust_dsp_ui.h>

#include <augr/rack/control/button.h>
#include <augr/rack/control/check_button.h>
#include <augr/rack/control/frame_box.h>
#include <augr/rack/control/h_bargraph.h>
#include <augr/rack/control/h_box.h>
#include <augr/rack/control/h_slider.h>
#include <augr/rack/control/knob.h>
#include <augr/rack/control/num_display.h>
#include <augr/rack/control/num_entry.h>
#include <augr/rack/control/tab_box.h>
#include <augr/rack/control/text_display.h>
#include <augr/rack/control/toggle_button.h>
#include <augr/rack/control/v_bargraph.h>
#include <augr/rack/control/v_box.h>
#include <augr/rack/control/v_slider.h>

namespace augr {

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

/*void FaustDspUi::declare(float* zone, const char* key, const char* value)
{
    map< int, list<strpair> >::iterator it = metadata_.find(nelems_);
    if (it != metadata_.end())
        it->second.push_back(strpair(key, value));
    else
        metadata_[nelems_] = list<strpair>(1, strpair(key, value));
}*/

void FaustDspUi::declare(float *zone, const char *key, const char *value) {
    map<FAUSTFLOAT *, Zone>::iterator it = zones_.find(zone);
    if (it != zones_.end())
        it->second.declare(key, value);
    else
        zones_[zone] = Zone();
}

void FaustDspUi::addButton(const char *label, float *zone) {
    AddModel(*new Button(label, zone));
}

void FaustDspUi::addToggleButton(const char *label, float *zone) {
    AddModel(*new ToggleButton(label, zone));
}

void FaustDspUi::addCheckButton(const char *label, float *zone) {
    AddModel(*new CheckButton(label, zone));
}

void FaustDspUi::addVerticalSlider(const char *label, float *zone, float init,
                                   float min, float max, float step) {
    if (zones_[zone].isKnob()) {
        return addKnob(label, zone, init, min, max, step);
    }

    AddModel(*new VSlider(label, zone, init, min, max, step));
}

void FaustDspUi::addHorizontalSlider(const char *label, float *zone, float init,
                                     float min, float max, float step) {
    if (zones_[zone].isKnob()) {
        return addKnob(label, zone, init, min, max, step);
    }

    AddModel(*new HSlider(label, zone, init, min, max, step));
}

void FaustDspUi::addKnob(const char *label, float *zone, float init, float min,
                         float max, float step) {
    AddModel(*new Knob(label, zone, init, min, max, step));
}

void FaustDspUi::addNumEntry(const char *label, float *zone, float init,
                             float min, float max, float step) {
    AddModel(*new NumEntry(label, zone, init, min, max, step));
}

void FaustDspUi::addNumDisplay(const char *label, float *zone, int precision) {
    AddModel(*new NumDisplay(label, zone, precision));
}

void FaustDspUi::addTextDisplay(const char *label, float *zone, char *names[],
                                float min, float max) {
    AddModel(*new TextDisplay(label, zone, names, min, max));
}

void FaustDspUi::addHorizontalBargraph(const char *label, float *zone,
                                       float min, float max) {
    AddModel(*new HBarGraph(label, zone, min, max));
}

void FaustDspUi::addVerticalBargraph(const char *label, float *zone, float min,
                                     float max) {
    AddModel(*new VBarGraph(label, zone, min, max));
}

void FaustDspUi::openFrameBox(const char *label) {
    PushModel(*new FrameBox(label));
}

void FaustDspUi::openTabBox(const char *label) {
    PushModel(*new TabBox(label));
}

void FaustDspUi::openHorizontalBox(const char *label) {
    auto box = new HBox(label);

    if (is_top_) {
        is_top_ = false;
        m_->label_ = label;
        box->is_top_level_ = true;
    }

    if (strcmp(label, "0x00") == 0) {
        box->label_visible_ = false;
    }

    PushModel(*box);
}

void FaustDspUi::openVerticalBox(const char *label) {
    auto box = new VBox(label);

    if (is_top_) {
        is_top_ = false;
        m_->label_ = label;
        box->is_top_level_ = true;
    }

    if (strcmp(label, "0x00") == 0) {
        box->label_visible_ = false;
    }

    PushModel(*box);
}

/*
void FaustDspUi::openHorizontalBox(const char* label) {
  if (is_top_) {
    is_top_ = false;
    m_->label_ = label;
    //return;
  }

  PushModel(*new HBox(label));
}

void FaustDspUi::openVerticalBox(const char* label) {
  if (is_top_) {
    is_top_ = false;
    m_->label_ = label;
    //return;
  }
  PushModel(*new VBox(label));
}
*/

void FaustDspUi::closeBox() {
    Model *top = PopModel();
    if (models_.empty())
        return;
    AddModel(*top);
}

} // namespace augr