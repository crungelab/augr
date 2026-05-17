#pragma once

#include <nlohmann/json.hpp>
#include <vector>

namespace augr {

class Vec2;
class Rack;
class RackView;
class Model;
class ModelWidget;

class RackSelection {
public:
    nlohmann::json BuildJson(Rack &rack, RackView &view,
                             const std::vector<Model *> &modules,
                             const std::vector<Widget *> &widgets);

    std::vector<Model *> MergeIntoRack(Rack &dest, RackView &view,
                                       const nlohmann::json &selection_json,
                                       Vec2 paste_offset);
};

} // namespace augr