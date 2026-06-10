// augite/archiver/module_widget_archiver.cpp
#include <augite/archiver/module_widget_archiver.h>

#include <augr/core/archive.h>
#include <augr/core/archiver_factory.h>
#include <nlohmann/json.hpp>

namespace augr {

void ModuleWidgetArchiver::Save(Archive& archive) const {
    auto& j = archive.json();
    const ModuleWidget& w = subject();

    j["type"] = factory_->name();

    j["grid_position"]   = { w.grid_position_.x,   w.grid_position_.y   };
    j["window_position"] = { w.window_position_.x, w.window_position_.y };
    j["window_size"]     = { w.window_size_.x,     w.window_size_.y     };
    j["window_open"]     = w.is_open_;
}

void ModuleWidgetArchiver::Load(Archive& archive) {
    const auto& j = archive.json();
    ModuleWidget& w = subject();

    auto read_vec2 = [](const nlohmann::json& jv, Vec2& out) {
        if (jv.is_array() && jv.size() == 2) {
            out.x = jv[0].get<float>();
            out.y = jv[1].get<float>();
        }
    };

    if (j.contains("grid_position"))   read_vec2(j["grid_position"],   w.grid_position_);
    if (j.contains("window_position")) read_vec2(j["window_position"], w.window_position_);
    if (j.contains("window_size"))     read_vec2(j["window_size"],     w.window_size_);
    if (j.contains("window_open"))     w.is_open_ = j["window_open"].get<bool>();
}

//DEFINE_ARCHIVER_FACTORY(ModuleWidgetArchiver, ModuleWidget, "Widget.Module")
DEFINE_ARCHIVER_FACTORY(ModuleWidgetArchiver, DefaultModuleWidget, "Widget.Module")

} // namespace augr