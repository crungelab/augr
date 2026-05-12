#pragma once

namespace augr {

class Model;
class ModelWidget;

class ModelWidgetBuilder {
public:
    virtual ModelWidget *Build(Model &model);
};

} // namespace augr