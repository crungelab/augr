#include "imgui.h"

#include "widget.h"

namespace augr {

template <typename T> class BoxWidgetT : public WidgetT<T> {
public:
    BoxWidgetT(T &model) : WidgetT<T>(model) {}

    void DrawHeader() {
        if (!this->model_->is_top_level_) {
            if(this->model_->label_visible_)
                ImGui::SeparatorText(this->model_->label_.c_str());
        }
    }

    // Data members
    ImDrawListSplitter dl_splitter_;
};

} // namespace augr