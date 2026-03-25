#pragma once

#include "plug_editor.h"

namespace Steinberg {
namespace Panner {

//-----------------------------------------------------------------------------
class PannerEditorImpl : public PannerEditor
{
public:
//------------------------------------------------------------------------
	// create function required for plug-in factory,
	// it will be called to create new instances of this controller
//------------------------------------------------------------------------
	static FUnknown* createInstance (void*)
	{
		return (Vst::IEditController*)new PannerEditorImpl();
	}
	virtual IPlugView* PLUGIN_API createView(const char* _name) override;
};

//------------------------------------------------------------------------
} // namespace Panner
} // namespace Steinberg
