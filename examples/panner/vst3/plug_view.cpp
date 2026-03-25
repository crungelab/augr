#include <iostream>

#include "plug_editor.h"
#include "plug_view.h"
#include "plug.h"

namespace Steinberg {
namespace Panner {


PannerView::PannerView(Panner::PannerEditor* editor, ViewRect* size)
    : PlugView(editor, size)
{
}

void PannerView::attachedToParent() {
    std::cout << "PannerView::attachedToParent" << std::endl << std::flush;
}

}}