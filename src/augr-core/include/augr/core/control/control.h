#pragma once

#include <string>
#include <vector>

#include <augr/core/config.h>
#include <augr/core/model.h>

namespace augr {

class Control : public Model {
public:
  Control() = default;
  Control(std::string label, fy_real* zone = nullptr) : label_(std::move(label)), zone_(zone) {}
  //Data members
  std::string label_;
  fy_real* zone_ = nullptr;

  REFLECT_ENABLE(Model)
};

} // namespace augr