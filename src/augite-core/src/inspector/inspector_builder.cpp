#include <augr/model.h>

#include <augite/inspector/inspector.h>
#include <augite/inspector/inspector_builder.h>
#include <augite/inspector/inspector_manufacturer.h>

namespace augr {

Widget::Ptr InspectorBuilder::Build(Model &model) {
    auto type = ::reflect::get_type(model);

    auto &manufacturer = InspectorManufacturer::singleton();

    ModelWidgetFactory *factory = manufacturer.GetFactory(type);
    Widget::Ptr widget = factory->Produce(model);
    /*
    for (const auto child : model.children_) {
        widget->AddChild(Build(*child));
    }
    */
    return widget;
}

void InspectorBuilder::BuildChildren(Widget& widget, Model &model) {
    for (const auto child : model.children_) {
        widget.AddChild(Build(*child));
    }
}

} // namespace augr