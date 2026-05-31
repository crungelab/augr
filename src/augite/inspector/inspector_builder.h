#pragma once

#include "./inspector.h"

namespace augr {

class Model;
class Widget;
class ModelWidget;

class InspectorBuilder {
public:
    virtual ModelWidget::Ptr Build(Model &model); // builds model + descendants
    virtual void BuildChildren(
        Widget &widget, Model &model); // builds container with descendants only
};

} // namespace augr