#pragma once

#include "view.h"

namespace augr {

class Model;
class Widget;

class ModelView : public View {
public:
  //virtual Model& model() = 0;
  virtual void Build();
  //Data members
  Widget* root_ = nullptr;
};

template<typename T>
class ModelViewT : public ModelView {
public:
  ModelViewT(T& model) : model_(&model) {}
  //virtual Model& model() override { return *model_; }
  virtual Model* model() override { return model_; }
  //Data members
  T* model_;
};

} // namespace augr