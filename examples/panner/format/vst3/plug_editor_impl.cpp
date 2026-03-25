#include "base/source/fstring.h"

#include "plug.h"
#include "plug_editor_impl.h"
#include "plug_view_impl.h"

namespace Steinberg {
namespace Panner {
//------------------------------------------------------------------------
IPlugView* PLUGIN_API PannerEditorImpl::createView(const char* _name)
{
	ConstString name (_name);
	if (name == Vst::ViewType::kEditor)
	{
		auto* view = new PlugViewImpl(this, new Steinberg::ViewRect(0,0,640,480));
		return view;
	}
	return nullptr;
}

//------------------------------------------------------------------------
} // namespace
} // namespace Steinberg
