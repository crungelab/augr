#pragma once

//#include "public.sdk/source/vst/vsteditcontroller.h"
//#include "vstgui/plugin-bindings/vst3editor.h"
#include <augr/core/vst3/plug_editor.h>

namespace Steinberg {
namespace Panner {

//-----------------------------------------------------------------------------
class PannerEditor : public PlugEditor
{
public:
//------------------------------------------------------------------------
	// create function required for plug-in factory,
	// it will be called to create new instances of this controller
//------------------------------------------------------------------------
	/*static FUnknown* createInstance(void*)
	{
		return (Vst::IEditController*)new PannerEditor ();
	}*/

	//---from IPluginBase--------
	tresult PLUGIN_API initialize (FUnknown* context) SMTG_OVERRIDE;

	//---from EditController-----
	//IPlugView* PLUGIN_API createView (const char* name) SMTG_OVERRIDE;
	tresult PLUGIN_API setComponentState (IBStream* state) SMTG_OVERRIDE;
};

//------------------------------------------------------------------------
} // namespace Panner
} // namespace Steinberg
