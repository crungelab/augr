#include <augr/core/archiver_factory.h>
#include <augr/core/archiver_manufacturer.h>

#include "subrack_viewer_archiver.h"

namespace augr {

/*
void SubrackViewerArchiver::Save(Archive &archive) const {
    //Archiver::Save(archive);
    ViewerArchiver::Save(archive);

    const SubrackViewer &viewer = subject();
    auto &j = archive.json();

    j["window_position"] = {viewer.window_position_.x,
                             viewer.window_position_.y};
    j["window_size"] = {viewer.window_size_.x, viewer.window_size_.y};

    SaveView(archive);
}

void SubrackViewerArchiver::SaveView(Archive &archive) const {
    const SubrackViewer &viewer = subject();
    if (!viewer.view_)
        return;

    auto &mfr = ArchiverManufacturer::singleton();
    auto *factory = mfr.FindFactory(std::type_index(typeid(*viewer.view_)));
    if (!factory) {
        std::cerr << "No archiver factory for SubrackView\n";
        return;
    }

    auto &view = const_cast<SubrackView &>(viewer.view());
    std::unique_ptr<Archiver> archiver(factory->Produce(view));

    auto &j = archive.json();
    JsonScope scope(archive, j["view"]);
    archiver->Save(archive);
}

void SubrackViewerArchiver::Load(Archive &archive) {
    SubrackViewer &viewer = subject();
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
}

void SubrackViewerArchiver::LoadView(Archive &archive) {
    SubrackViewer &viewer = subject();
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
*/
DEFINE_ARCHIVER_FACTORY(SubrackViewerArchiver, SubrackViewer, "SubrackViewer")

} // namespace augr