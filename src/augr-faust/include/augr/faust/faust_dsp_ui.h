#pragma once

#include <cmath>
#include <cstdint>
#include <cstdlib>
#include <list>
#include <map>
#include <set>
#include <vector>

#include <augr/core/control/control_meta.h>

#include "faust_ui.h"

using namespace std;

using Meta = augr::Meta;

namespace augr {

using ZoneMetaMap = map<FAUSTFLOAT *, ControlMeta>;

class Model;
class Parameter;
class FaustDsp;

typedef pair<const char *, const char *> strpair;

enum SliderKind { Vertical, Horizontal };

class FaustDspUi : public UI {
public:
    FaustDspUi(FaustDsp &m);
    void PushModel(Model &model);
    Model *PopModel();
    void AddModel(Model &model);

    Parameter *MakeParameter(const char *label, float *zone, fy_real init,
                             fy_real min, fy_real max, fy_real step);
    // Builder methods
    void declare(float *zone, const char *key, const char *value) override;

    void addButton(const char *label, float *zone) override;
    void addToggleButton(const char *label, float *zone) override;
    void addCheckButton(const char *label, float *zone) override;
    void addKnob(const char *label, float *zone, float init, float min,
                 float max, float step);
    void addVerticalSlider(const char *label, float *zone, float init,
                           float min, float max, float step) override;
    void addHorizontalSlider(const char *label, float *zone, float init,
                             float min, float max, float step) override;
    void addNumEntry(const char *label, float *zone, float init, float min,
                     float max, float step) override;

    // -- passive widgets
    void addNumDisplay(const char *label, float *zone, int precision) override;
    void addTextDisplay(const char *label, float *zone, char *names[],
                        float min, float max) override;
    void addHorizontalBargraph(const char *label, float *zone, float min,
                               float max) override;
    void addVerticalBargraph(const char *label, float *zone, float min,
                             float max) override;

    // -- frames and labels
    void openFrameBox(const char *label) override;
    void openTabBox(const char *label) override;
    void openHorizontalBox(const char *label) override;
    void openVerticalBox(const char *label) override;
    void closeBox() override;

    // Data members
    FaustDsp *m_;
    map<int, list<strpair>> metadata_;
    ZoneMetaMap zones_;
    std::vector<Model *> models_;
    bool is_top_ = true;
};

} // namespace augr