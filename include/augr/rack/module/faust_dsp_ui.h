#pragma once

#include <cstdlib>
#include <cstdint>
#include <cmath>
#include <list>
#include <map>
#include <set>
#include <vector>

#include "faust_ui.h"

using namespace std;


struct Meta : std::map<const char *, const char *>
{
  void declare(const char *key, const char *value)
  {
    (*this)[key] = value;
  }
  const char *get(const char *key, const char *def)
  {
    if (this->find(key) != this->end())
      return (*this)[key];
    else
      return def;
  }
};

namespace augr {

class Model;

class FaustDsp;

typedef pair<const char *, const char *> strpair;

struct Zone {
  std::map<std::string, std::string> data;

  void declare(const char* key, const char* value) {
    data[std::string(key)] = std::string(value);
  }

  // non-inserting get; returns pointer for cheap use-or-null
  const std::string* get_ptr(const char* key) const {
    auto it = data.find(key);                 // std::string find
    return it == data.end() ? nullptr : &it->second;
  }

  // If you really want a C-string, expose a view (safer than owning char*)
  const char* get(const char* key) const {
    if (auto s = get_ptr(key)) return s->c_str();
    return nullptr;
  }

  bool isKnob() const {
    if (auto s = get_ptr("style")) return *s == "knob";
    return false;
  }
};

enum SliderKind {
  Vertical,
  Horizontal
};

class FaustDspUi : public UI
{
public:
  FaustDspUi(FaustDsp &m);
  void PushModel(Model &model);
  Model *PopModel();
  void AddModel(Model &model);
  // Builder methods
  void declare(float *zone, const char *key, const char *value) override;

  void addButton(const char *label, float *zone) override;
  void addToggleButton(const char *label, float *zone) override;
  void addCheckButton(const char *label, float *zone) override;
  void addKnob(const char *label, float *zone, float init, float min, float max, float step);
  void addVerticalSlider(const char *label, float *zone, float init, float min, float max, float step) override;
  void addHorizontalSlider(const char *label, float *zone, float init, float min, float max, float step) override;
  void addNumEntry(const char *label, float *zone, float init, float min, float max, float step) override;

  // -- passive widgets
  void addNumDisplay(const char *label, float *zone, int precision) override;
  void addTextDisplay(const char *label, float *zone, char *names[], float min, float max) override;
  void addHorizontalBargraph(const char *label, float *zone, float min, float max) override;
  void addVerticalBargraph(const char *label, float *zone, float min, float max) override;

  // -- frames and labels
  void openFrameBox(const char *label) override;
  void openTabBox(const char *label) override;
  void openHorizontalBox(const char *label) override;
  void openVerticalBox(const char *label) override;
  void closeBox() override;

  // Data members
  FaustDsp *m_;
  map<int, list<strpair>> metadata_;
  map<FAUSTFLOAT*, Zone> zones_;
  int nelems_, nports_;
  std::vector<Model *> models_;
  bool is_top_ = true;
};

} // namespace augr