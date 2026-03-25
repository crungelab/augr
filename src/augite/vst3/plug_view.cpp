#include "plug_editor.h"
#include "plug_view.h"

using namespace Steinberg;

PlugView::PlugView (PlugEditor* editor, ViewRect* size)
: Steinberg::Vst::EditorView(editor, size)
{
}

tresult PLUGIN_API PlugView::isPlatformTypeSupported (FIDString type) {
    if (std::strcmp(type, Steinberg::kPlatformTypeX11EmbedWindowID) == 0)
        return kResultTrue;
    return kResultFalse;
}