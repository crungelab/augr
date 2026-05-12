#pragma once

#include "widget.h"

namespace augr {

class Rack;

class RackWidget : public ModelWidgetT<Rack> {
public:
  RackWidget(Rack& model) : ModelWidgetT<Rack>(model) {}
  //virtual void Draw();
};

} // namespace augr