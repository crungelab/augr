// ModuleArchiver.cpp
#include <augr/core/archiver_manufacturer.h>

#include <augr/rack/archiver/graph_archiver.h>
#include <augr/rack/graph.h>

#include <augr/core/archiver_factory.h>
#include <nlohmann/json.hpp>

namespace augr {

void SerializeModel(Archive &archive, Model &model) {
    auto &manufacturer = ArchiverManufacturer::singleton();

    // Look up the archiver factory for this module's dynamic type.
    auto *factory = manufacturer.FindFactory(std::type_index(typeid(model)));
    if (!factory) {
        std::cerr << "No archiver factory registered for type "
                  << typeid(model).name() << "\n";
        return;
    }

    // Produce an archiver bound to this module.
    auto archiver = factory->Produce(model);
    if (!archiver) {
        std::cerr << "Failed to construct archiver\n";
        return;
    }

    archiver->Save(archive);
    delete archiver;
}

void GraphArchiver::Save(Archive &archive) const {
    ModuleArchiver::Save(archive);

    for (auto &child : model().children_) {
        nlohmann::json j_child;
        Archive child_archive(j_child, archive.version());
        SerializeModel(child_archive, *child);
        archive.json()["children"].push_back(j_child);
    }
}

void GraphArchiver::Load(Archive &archive) {
    const auto &j = archive.json();
    Module &module = model();
}

} // namespace augr

using namespace augr;

DEFINE_ARCHIVER_FACTORY(GraphArchiver, Graph, "Graph")