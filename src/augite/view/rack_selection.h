#pragma once

#include <vector>
#include <nlohmann/json.hpp>


namespace augr {

class Vec2;
class Rack;
class RackView;
class Model;
class ModelWidget;

class RackSelection {
public:

nlohmann::json BuildSelectionJson(Rack &rack, RackView &view,
                                  const std::vector<Model *> &modules,
                                  const std::vector<Widget *> &widgets);


std::vector<Model *>
MergeSelectionIntoRack(Rack &dest, RackView &view,
                       const nlohmann::json &selection_json,
                       Vec2 paste_offset);
};

} // namespace augr