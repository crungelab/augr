// ModuleArchiver.cpp
#include <augr/core/archiver_manufacturer.h>

#include <augr/rack/graph.h>
#include <augr/rack/wire.h>

#include <augr/rack/archiver/graph_archiver.h>

#include <augr/core/archiver_factory.h>
#include <nlohmann/json.hpp>

namespace augr {

void GraphArchiver::Save(Archive &archive) const {
    ModuleArchiver::Save(archive);

    const Graph &graph = model();

    // Push graph context so children iteration AND wire resolution
    // both see this graph's children as the resolution table.
    GraphScope graph_scope(archive, graph.children_);

    SaveChildren(archive);
    SaveWires(archive);
}

void GraphArchiver::SaveChildren(Archive &archive) const {

    auto &j = archive.json();
    const Graph &graph = model();

    if (graph.children_.empty())
        return;

    auto &j_children = j["children"] = nlohmann::json::array();
    for (auto *child : graph.children_) {
        nlohmann::json j_child = nlohmann::json::object();
        {
            JsonScope json_scope(archive, j_child);
            ArchiverManufacturer::singleton().Serialize(archive, *child);
        }
        j_children.push_back(std::move(j_child));
    }
}

void GraphArchiver::SaveWires(Archive &archive) const {
    const Graph &graph = model();
    if (graph.wires_.empty())
        return;

    auto &j = archive.json();
    auto &j_wires = j["wires"] = nlohmann::json::array();

    for (const Wire *wire : graph.wires_) {
        const Pin *out_pin = wire->output_;
        const Pin *in_pin = wire->input_;

        // Resolve each pin's owning Module and look up its index in the
        // current graph's children. If either lookup fails, the wire is
        // dangling — skip it.
        auto &out_module = out_pin->node();
        auto &in_module = in_pin->node();

        int out_idx = archive.IndexOf(&out_module);
        int in_idx = archive.IndexOf(&in_module);

        if (out_idx < 0 || in_idx < 0) {
            // Wire references something outside this graph's children —
            // drop it silently per the "dropped" connection policy.
            continue;
        }

        nlohmann::json j_wire = nlohmann::json::array();
        j_wire.push_back(nlohmann::json::array({out_idx, out_pin->name()}));
        j_wire.push_back(nlohmann::json::array({in_idx, in_pin->name()}));
        j_wires.push_back(std::move(j_wire));
    }

    // If we ended up with no valid wires (all dropped), remove the empty
    // array to keep the output clean. Optional.
    if (j_wires.empty()) {
        j.erase("wires");
    }
}

void GraphArchiver::Load(Archive &archive) {
    const auto &j = archive.json();
    Module &module = model();
}

} // namespace augr

using namespace augr;

DEFINE_ARCHIVER_FACTORY(GraphArchiver, Graph, "Graph")