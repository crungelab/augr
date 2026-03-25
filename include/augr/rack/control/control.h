#pragma once

#include <vector>

#include <augr/config.h>
#include <augr/rack/model.h>

namespace augr {

class Control : public Model {
public:
  Control() : label_(nullptr), zone_(nullptr) {}
  Control(const char* label, fy_real* zone = nullptr) : label_(label), zone_(zone) {}
  //Data members
  const char* label_;
  fy_real* zone_;

  REFLECT_ENABLE(Model)
};

} // namespace augr