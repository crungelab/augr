#pragma once

#include <augite/widget/widget.h>

namespace augr {

class Rack;

class RackWidget : public ModelWidgetT<Rack> {
public:
    RackWidget(Rack &model) : ModelWidgetT<Rack>(model) {}
};

} // namespace augr