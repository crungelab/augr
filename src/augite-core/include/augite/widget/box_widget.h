#include "imgui.h"

#include <augite/widget/widget.h>

namespace augr {

template <typename T> class BoxWidgetT : public ModelWidgetT<T> {
public:
    BoxWidgetT(T &model) : ModelWidgetT<T>(model) {}

    void DrawHeader() {
        auto &m = this->model();
        if (!m.is_top_level_) {
            if(m.label_visible_)
                ImGui::SeparatorText(m.label_.c_str());
        }
    }

    // Data members
    ImDrawListSplitter dl_splitter_;
};

} // namespace augr