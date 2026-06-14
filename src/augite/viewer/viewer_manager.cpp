#include <iostream>

#include "viewer.h"
#include "viewer_factory.h"
#include "viewer_manager.h"
#include "viewer_manufacturer.h"

#include "../app/app.h"

namespace augr {

/*
void ViewerManager::ToggleViewer(Frame &parent, Document &document,
                                 Model &model) {
    if (auto it = viewers_.find(&model); it != viewers_.end()) {
        CloseViewer(*it->second);
    } else {
        const auto &mfr = ViewerManufacturer::singleton();
        auto *factory = mfr.FindFactory(std::type_index(typeid(model)));
        if (!factory) {
            std::cerr << "No viewer factory for model type: "
                      << typeid(model).name() << "\n";
            return;
        }
        //Viewer::Ptr viewer = factory->Produce("Viewer", document, model);
        Viewer::Ptr viewer = factory->Produce(model.label(), document, model);
        viewer->Create();
        viewers_[&model] = static_cast<Viewer *>(viewer.get());
        parent.AddChild(std::move(viewer)); // parent takes ownership
    }
}
*/

void ViewerManager::ToggleViewer(Frame &parent, Document &document,
                                 Model &model) {
    if (auto it = viewers_.find(&model); it != viewers_.end()) {
        CloseViewer(*it->second);
    } else {
        OpenViewer(parent, document, model);
    }
}

void ViewerManager::OpenViewer(Frame &parent, Document &document, Model &model) {
    if (auto it = viewers_.find(&model); it != viewers_.end()) {
        CloseViewer(*it->second);
    } else {
        const auto &mfr = ViewerManufacturer::singleton();
        auto *factory = mfr.FindFactory(std::type_index(typeid(model)));
        if (!factory) {
            std::cerr << "No viewer factory for model type: "
                      << typeid(model).name() << "\n";
            return;
        }
        //Viewer::Ptr viewer = factory->Produce("Viewer", document, model);
        Viewer::Ptr viewer = factory->Produce(model.label(), document, model);
        viewer->Create();
        viewers_[&model] = static_cast<Viewer *>(viewer.get());
        parent.AddChild(std::move(viewer)); // parent takes ownership
    }
}

/*
TODO: This crashed once.  Might need to wrap in App::singleton().QueueAction()
*/

void ViewerManager::CloseViewer(Viewer &viewer) {
    viewer.Destroy();
    // Find the viewer in the map and remove it.
    auto it =
        std::find_if(viewers_.begin(), viewers_.end(),
                     [&](const auto &pair) { return pair.second == &viewer; });
    if (it != viewers_.end()) {
        viewers_.erase(it);
    }
}

} // namespace augr