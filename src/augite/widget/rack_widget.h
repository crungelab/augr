#pragma once

#include "widget.h"

namespace augr {

class Rack;

class RackWidget : public WidgetT<Rack> {
public:
  RackWidget(Rack& model) : WidgetT<Rack>(model) {}
  //virtual void Draw();
};

} // namespace augr