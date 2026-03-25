#pragma once

namespace augr {

class Model;
class Widget;

class WidgetBuilder {
public:
  virtual Widget* Build(Model& model);
};

} // namespace augr