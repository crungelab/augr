// augite/archiver/rack_view_archiver.cpp
#include <iostream>

#include <augite/archiver/subrack_view_archiver.h>

#include <augr/core/archive.h>
#include <augr/core/archiver_factory.h>
#include <augr/core/archiver_manufacturer.h>

#include <augite/widget/module_widget.h>
#include <augite/widget/widget_builder.h>

#include <nlohmann/json.hpp>

namespace augr {

void SubrackViewArchiver::Save(Archive& archive) const {
    const SubrackView& view = subject();

    auto& j = archive.json();
    j["type"] = factory_->type_name();

    // View-level state (pan, zoom, etc.) goes here in the future.
    // For now, just the widgets array.

    SaveWidgets(archive, view.root_->children_);

}

void SubrackViewArchiver::SaveWidgets(Archive& archive, const std::vector<Widget *> &widgets) const {
    const SubrackView& view = subject();

    auto& j = archive.json();

    if (view.root_ == nullptr || view.root_->children_.empty()) return;

    auto& j_widgets = j["widgets"] = nlohmann::json::array();

    for (Widget* child : widgets) {
        nlohmann::json j_widget = nlohmann::json::object();
        {
            JsonScope scope(archive, j_widget);
            ArchiverManufacturer::singleton().Serialize(archive, *child);
        }
        j_widgets.push_back(std::move(j_widget));
    }
}

void SubrackViewArchiver::Load(Archive& archive) {
    LoadWidgets(archive);
}

void SubrackViewArchiver::LoadWidgets(Archive& archive) {
    const auto& j = archive.json();
    SubrackView& view = subject();

    if (!j.contains("widgets") || !j["widgets"].is_array()) return;
    const auto& j_widgets = j["widgets"];

    // Assumption: the rack has already been loaded, view.root_ has been
    // built via SubrackView::Build() — so widgets already exist parallel to
    // the rack's children. We just deserialize state into them.
    //
    // Sanity check: widget count must match. If not, log and load what
    // we can.
    if (view.root_ == nullptr) {
        // No build has happened yet. Caller error — load is being called
        // out of order. Bail.
        return;
    }

    const auto& children = view.root_->children_;
    const size_t n = std::min(children.size(), j_widgets.size());
    if (children.size() != j_widgets.size()) {
        // Could mean: rack loaded different children than the view JSON
        // expected (file from an older format, or modules failed to load).
        // Load the overlap; warn.
        std::cerr << "SubrackView load: widget count mismatch (have "
                  << children.size() << ", json has " << j_widgets.size()
                  << "). Loading first " << n << ".\n";
    }

    auto& mfr = ArchiverManufacturer::singleton();
    for (size_t i = 0; i < n; ++i) {
        const auto& j_widget = j_widgets[i];
        if (!j_widget.contains("type")) continue;

        // Optional: sanity-check the type tag matches what the widget
        // actually is. For now, just deserialize into whatever's there.
        JsonScope scope(archive, const_cast<nlohmann::json&>(j_widget));
        mfr.Deserialize(archive, *children[i]);
    }
}

DEFINE_ARCHIVER_FACTORY(SubrackViewArchiver, SubrackView, "SubrackView")

} // namespace augr
