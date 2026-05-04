// ModuleArchiver.cpp
#include <augr/core/archiver_manufacturer.h>

#include <augr/rack/archiver/graph_archiver.h>
#include <augr/rack/graph.h>

#include <augr/core/archiver_factory.h>
#include <nlohmann/json.hpp>

namespace augr {

void GraphArchiver::Save(Archive &archive) const {
    ModuleArchiver::Save(archive);

    auto &j = archive.json();
    const Graph &graph = model();

    if (graph.children_.empty())
        return;

    // Push graph context so connections in this graph resolve correctly.
    GraphScope graph_scope(archive,
                           graph.children_); // copy of children pointers

    auto &j_children = j["children"] = nlohmann::json::array();
    for (auto *child : graph.children_) {
        nlohmann::json j_child = nlohmann::json::object();
        {
            JsonScope json_scope(archive, j_child);
            ArchiverManufacturer::singleton().Serialize(archive, *child);
        }
        j_children.push_back(std::move(j_child));
    }

    // Connections would be saved here, using archive.IndexOf(...).
}

void GraphArchiver::Load(Archive &archive) {
    const auto &j = archive.json();
    Module &module = model();
}

} // namespace augr

using namespace augr;

DEFINE_ARCHIVER_FACTORY(GraphArchiver, Graph, "Graph")