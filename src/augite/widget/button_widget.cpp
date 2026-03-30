#include "imgui.h"

#include "widget.h"

#include <augr/core/rack/control/button.h>
#include <augr/core/rack/rack.h>

namespace augr {

class ButtonWidget : public WidgetT<Button> {
public:
    ButtonWidget(Button &model) : WidgetT<Button>(model) {}

    void Draw() override {
        ImGui::Button("Gate");
        float gate = ImGui::IsItemActive() ? 1.0f : 0.0f;
        *model_->zone_ = gate;
    }

    /*
    void Draw() override {
        if (ImGui::Button(model_->label_)) {
            auto zone = model_->zone_;
            Rack::singleton().EnqueueAction([zone]() { *zone = 1.0f; },
                                           [zone]() { *zone = 0.0f; });
        }
    }
    */
    /*
    void Draw() override {
      if (ImGui::Button(model_->label_)) {
        *model_->zone_ = 1.0;
      }
      else {
        *model_->zone_ = 0.0;
      }
    }
    */
};
DEFINE_WIDGET_FACTORY(ButtonWidget, Button)

} // namespace augr