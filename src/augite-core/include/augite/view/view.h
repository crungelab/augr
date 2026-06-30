#pragma once

#include "../widget/model_widget.h"

namespace augr {

// View: ModelWidget + the view-layer concerns that are real
class View : public ModelWidget {
public:
    View(Model &model) : ModelWidget(model) {}
    virtual ~View() = default;

    virtual void Build();

    Widget *root_ = nullptr;
};

template <typename T, typename TBase = View>
using ViewT = ModelWidgetT<T, TBase>;

} // namespace augr