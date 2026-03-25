#include "imgui.h"

#include "widget.h"

namespace augr {

template <typename T> class BoxWidgetT : public WidgetT<T> {
public:
    BoxWidgetT(T &model) : WidgetT<T>(model) {}

    void DrawHeader() {
        if (!this->model_->is_top_level_) {
            if(this->model_->label_visible_)
                ImGui::SeparatorText(this->model_->label_);
        }
    }

    /*
    void DrawHeader() {
        if (!this->model_->is_top_level_) {
            if(this->model_->label_visible_)
                ImGui::SeparatorText(this->model_->label_);
            else {
                ImGui::Dummy(ImVec2(0.0f, 5.0f));
                ImGui::Separator();
                ImGui::Dummy(ImVec2(0.0f, 5.0f));
            }
        }
    }
    */

    // Data members
    ImDrawListSplitter dl_splitter_;
};

} // namespace augr