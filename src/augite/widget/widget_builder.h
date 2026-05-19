#pragma once

namespace augr {

class Model;
class Widget;
class ModelWidget;

class ModelWidgetBuilder {
public:
    // virtual ModelWidget *Build(Model &model);
    virtual ModelWidget *Build(Model &model); // builds model + descendants
    virtual void BuildChildren(
        Widget &widget, Model &model); // builds container with descendants only
};

} // namespace augr