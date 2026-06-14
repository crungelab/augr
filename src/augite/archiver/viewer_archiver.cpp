#include <iostream>

#include <augr/core/archiver_factory.h>
#include <augr/core/archiver_manufacturer.h>
#include <augr/core/model_registry.h>

#include <augite/app/app.h>

#include "viewer_archiver.h"

namespace augr {

void ViewerArchiver::Save(Archive &archive) const {
    Archiver::Save(archive);

    const Viewer &viewer = subject();
    auto &j = archive.json();

    j["window_position"] = {viewer.window_position_.x,
                            viewer.window_position_.y};
    j["window_size"] = {viewer.window_size_.x, viewer.window_size_.y};

    SaveView(archive);
    SaveSubviewers(archive);
}

void ViewerArchiver::SaveView(Archive &archive) const {
    const Viewer &viewer = subject();
    if (!viewer.view_)
        return;

    auto &mfr = ArchiverManufacturer::singleton();
    auto *factory = mfr.FindFactory(std::type_index(typeid(*viewer.view_)));
    if (!factory) {
        std::cerr << "No archiver factory for SubrackView\n";
        return;
    }

    auto &view = const_cast<View &>(viewer.view());
    std::unique_ptr<Archiver> archiver(factory->Produce(view));

    auto &j = archive.json();
    JsonScope scope(archive, j["view"]);
    archiver->Save(archive);
}

void ViewerArchiver::SaveSubviewers(Archive &archive) const {
    const Viewer &viewer = subject();
    auto &j = archive.json();

    auto &j_subviewers = j["subviewers"];
    j_subviewers = nlohmann::json::array();

    for (const auto &child : viewer.children_) {
        auto *child_viewer = dynamic_cast<Viewer *>(child.get());
        if (!child_viewer)
            continue;
        j_subviewers.push_back(child_viewer->model().uuid_to_string());
    }
}

void ViewerArchiver::Load(Archive &archive) {
    Viewer &viewer = subject();
    auto &j = archive.json();

    auto read_vec2 = [](const nlohmann::json &jv, Vec2 &out) {
        if (jv.is_array() && jv.size() == 2) {
            out.x = jv[0].get<float>();
            out.y = jv[1].get<float>();
        }
    };

    if (j.contains("window_position"))
        read_vec2(j["window_position"], viewer.window_position_);
    if (j.contains("window_size"))
        read_vec2(j["window_size"], viewer.window_size_);

    LoadView(archive);
    LoadSubviewers(archive);
}

void ViewerArchiver::LoadView(Archive &archive) {
    Viewer &viewer = subject();
    if (!viewer.view_)
        return;

    auto &j = archive.json();
    if (!j.contains("view"))
        return;

    auto &mfr = ArchiverManufacturer::singleton();
    auto *factory = mfr.FindFactory(std::type_index(typeid(*viewer.view_)));
    if (!factory)
        return;

    std::unique_ptr<Archiver> archiver(factory->Produce(*viewer.view_));

    JsonScope scope(archive, j["view"]);
    archiver->Load(archive);
}

void ViewerArchiver::LoadSubviewers(Archive &archive) {
    Viewer &viewer = subject();
    const auto &j = archive.json();

    if (!j.contains("subviewers") || !j["subviewers"].is_array())
        return;

    for (const auto &j_uuid : j["subviewers"]) {
        if (!j_uuid.is_string())
            continue;
        auto uuid_str = j_uuid.get<std::string>();
        auto model_uuid = uuids::uuid::from_string(uuid_str);
        if (!model_uuid)
            continue;
        auto model = ModelRegistry::singleton().Find(model_uuid.value());
        if (!model)
            continue;
        auto &vm = App::singleton().viewer_manager();
        vm.OpenViewer(viewer, viewer.document(), *model);
    }
}

DEFINE_ARCHIVER_FACTORY(ViewerArchiver, Viewer, "Viewer")

} // namespace augr