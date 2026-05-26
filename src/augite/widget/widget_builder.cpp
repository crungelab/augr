#include <augr/core/model.h>

#include "widget.h"
#include "widget_builder.h"
#include "widget_manufacturer.h"

namespace augr {

Widget::Ptr ModelWidgetBuilder::Build(Model &model) {
    auto type = ::reflect::get_type(model);

    auto &manufacturer = ModelWidgetManufacturer::singleton();

    ModelWidgetFactory *factory = manufacturer.GetFactory(type);
    Widget::Ptr widget = factory->Produce(model);
    for (const auto child : model.children_) {
        widget->AddChild(Build(*child));
    }
    return widget;
}

void ModelWidgetBuilder::BuildChildren(Widget& widget, Model &model) {
    for (const auto child : model.children_) {
        widget.AddChild(Build(*child));
    }
}

} // namespace augr