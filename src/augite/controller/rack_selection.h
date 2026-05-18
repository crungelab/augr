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
    static nlohmann::json
    BuildSelectionJson(Rack &rack, RackView &view,
                       const std::vector<Model *> &modules,
                       const std::vector<Widget *> &widgets);

    static std::vector<Model *>
    MergeIntoRack(Rack &dest, RackView &view,
                  const nlohmann::json &selection_json, Vec2 paste_offset);

    static bool LooksLikeSelection(const nlohmann::json &payload);
};

} // namespace augr