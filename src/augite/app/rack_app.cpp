#include "imgui.h"

#include "imnodes.h"
#include "implot.h"

#include "../widget/widget_manufacturer.h"

#include "../system/imgui_system.h"
#include "../system/imnodes_system.h"
#include "../system/implot_system.h"

#include "../event/event.h"

#include "rack_app.h"

#include "../view/rack_view.h"

namespace augr {

RackApp *RackApp::singleton_;

RackApp::RackApp() {
    singleton_ = this;
    doc_.NewDocument();

    view_ = new RackView(rack());
};

} // namespace augr