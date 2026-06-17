#include <augr/archiver_factory.h>
#include <augr/archiver_manufacturer.h>
//#include <augr/model_registry.h>

#include <augite/app/app.h>

#include <augite/archiver/subrack_viewer_archiver.h>

namespace augr {

/*
void SubrackViewerArchiver::Save(Archive &archive) const {
    ViewerArchiver::Save(archive);

    SaveSubviewers(archive);
}

void SubrackViewerArchiver::SaveSubviewers(Archive &archive) const {
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

void SubrackViewerArchiver::Load(Archive &archive) {
    ViewerArchiver::Load(archive);
    LoadSubviewers(archive);
}

void SubrackViewerArchiver::LoadSubviewers(Archive &archive) {
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
*/

DEFINE_ARCHIVER_FACTORY(SubrackViewerArchiver, SubrackViewer, "SubrackViewer")

} // namespace augr