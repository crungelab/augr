#include <unordered_set>

#include <augr/archiver_manufacturer.h>
#include <augr/model_manufacturer.h>

#include <augr/rack/graph.h>
#include <augr/rack/wire.h>

#include <augr/rack/archiver/graph_archiver.h>

#include <augr/archiver_factory.h>
#include <nlohmann/json.hpp>

namespace augr {

void GraphArchiver::Save(Archive &archive) const {
    ModuleArchiver::Save(archive);

    const Graph &graph = subject();
    auto &j = archive.json();

    GraphScope graph_scope(archive, graph.children_);

    SaveChildren(archive, graph.children_);
    SaveWires(archive, graph.children_);
}

void GraphArchiver::SaveChildren(
    Archive &archive, const std::vector<Model::Ptr> &children) const {
    auto &j = archive.json();

    if (children.empty())
        return;

    auto &j_children = j["children"] = nlohmann::json::array();
    for (const auto &child : children) {
        if (child->is_replicated())
            continue;
        nlohmann::json j_child = nlohmann::json::object();
        {
            JsonScope json_scope(archive, j_child);
            ArchiverManufacturer::singleton().Serialize(archive, *child);
        }
        j_children.push_back(std::move(j_child));
    }
}

void GraphArchiver::SaveWires(Archive &archive,
                              const std::vector<Model::Ptr> &children) const {
    const Graph &graph = subject();
    if (graph.wires_.empty())
        return;

    GraphScope graph_scope(archive, children);

    auto &j = archive.json();
    auto &j_wires = j["wires"] = nlohmann::json::array();

    for (const Wire *wire : graph.wires_) {
        const Pin *out_pin = wire->output_;
        const Pin *in_pin = wire->input_;

        auto &out_module = out_pin->node();
        auto &in_module = in_pin->node();

        int out_idx = archive.IndexOf(&out_module);
        int in_idx = archive.IndexOf(&in_module);

        if (out_idx < 0 || in_idx < 0)
            continue;

        nlohmann::json j_wire = nlohmann::json::array();
        j_wire.push_back(nlohmann::json::array({out_idx, out_pin->name()}));
        j_wire.push_back(nlohmann::json::array({in_idx, in_pin->name()}));
        j_wires.push_back(std::move(j_wire));
    }

    if (j_wires.empty())
        j.erase("wires");
}

void GraphArchiver::Load(Archive &archive) {
    ModuleArchiver::Load(archive);

    Graph &graph = subject();
    const auto &j = archive.json();

    LoadChildren(archive);

    GraphScope graph_scope(archive, graph.children_);

    LoadWires(archive);
}

void GraphArchiver::LoadChildren(Archive &archive) {
    const auto &j = archive.json();
    Graph &graph = subject();

    if (!j.contains("children"))
        return;
    const auto &j_children = j["children"];
    if (!j_children.is_array())
        return;

    auto &model_manufacturer = ModelManufacturer::singleton();
    auto &archiver_manufacturer = ArchiverManufacturer::singleton();

    for (const auto &j_child : j_children) {
        if (!j_child.contains("type")) {
            std::cerr << "Child missing 'type' field — skipping\n";
            continue;
        }
        auto type_name = j_child["type"].get<std::string>();

        ModelFactory *factory = model_manufacturer.FindFactory(type_name);
        if (!factory) {
            std::cerr << "Unknown module type '" << type_name
                      << "' — skipping\n";
            continue;
        }

        Model &child =
            *factory->Produce(graph.shared_from_this(), CreateMode::Loaded);

        JsonScope json_scope(archive, const_cast<nlohmann::json &>(j_child));
        archiver_manufacturer.Deserialize(archive, child);
    }
}

void GraphArchiver::LoadWires(Archive &archive) {
    const auto &j = archive.json();
    Graph &graph = subject();

    if (!j.contains("wires"))
        return;
    const auto &j_wires = j["wires"];
    if (!j_wires.is_array())
        return;

    for (const auto &j_wire : j_wires) {
        if (!j_wire.is_array() || j_wire.size() != 2)
            continue;
        const auto &j_from = j_wire[0];
        const auto &j_to = j_wire[1];
        if (!j_from.is_array() || j_from.size() != 2)
            continue;
        if (!j_to.is_array() || j_to.size() != 2)
            continue;

        int from_idx = j_from[0].get<int>();
        std::string from_pin_name = j_from[1].get<std::string>();
        int to_idx = j_to[0].get<int>();
        std::string to_pin_name = j_to[1].get<std::string>();

        Model *from_module = archive.ResolveModule(from_idx);
        Model *to_module = archive.ResolveModule(to_idx);
        if (!from_module || !to_module)
            continue;

        Node *from_node = dynamic_cast<Node *>(from_module);
        Node *to_node = dynamic_cast<Node *>(to_module);
        if (!from_node || !to_node) {
            std::cerr << "Wire endpoint is not a Node — skipping\n";
            continue;
        }

        Pin *from_pin = from_node->outport_.FindPin(from_pin_name);
        Pin *to_pin = to_node->inport_.FindPin(to_pin_name);
        if (!from_pin || !to_pin) {
            std::cerr << "Wire references unknown pin "
                      << (from_pin ? to_pin_name : from_pin_name)
                      << " — skipping\n";
            continue;
        }

        graph.Connect(*from_pin, *to_pin);
    }
}

} // namespace augr

using namespace augr;
DEFINE_ARCHIVER_FACTORY(GraphArchiver, Graph, "Graph")