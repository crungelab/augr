#include <augr/core/rack/model.h>

#include "widget.h"
#include "widget_builder.h"
#include "widget_manufacturer.h"

namespace augr {

Widget *WidgetBuilder::Build(Model &model) {
    auto type = ::reflect::get_type(model);

    auto manufacturer = WidgetManufacturer::singleton();

    WidgetFactory *factory = manufacturer.GetFactory(type);
    Widget *widget = factory->Produce(model);
    for (const auto child : model.children_) {
        widget->AddChild(Build(*child));
    }
    return widget;
}

} // namespace augr