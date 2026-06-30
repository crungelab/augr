#pragma once

#include <map>

namespace augr {


class Document;
class Model;
class Viewer;
class Frame;

class ViewerManager {
public:
    void ToggleViewer(Frame &parent, Document &document, Model &model);
    void OpenViewer(Frame &parent, Document &document, Model &model);
    void CloseViewer(Viewer &viewer);
    void CloseViewerFor(Model &model);

    // Data members
    std::map<Model *, Viewer *> viewers_;
};

} // namespace augr