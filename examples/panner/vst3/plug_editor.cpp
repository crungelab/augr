#include "plug.h"
#include "parameters.h"
#include "plug_editor.h"

#include "base/source/fstreamer.h"
#include "base/source/fstring.h"
#include "pluginterfaces/base/ibstream.h"
#include "pluginterfaces/base/ustring.h"
#include "public.sdk/source/vst/vstparameters.h"

namespace Steinberg {
namespace Panner {

// example of custom parameter (overwriting to and fromString)
//------------------------------------------------------------------------
class PanParameter : public Vst::Parameter
{
public:
	PanParameter (int32 flags, int32 id);

	void toString (Vst::ParamValue normValue, Vst::String128 string) const SMTG_OVERRIDE;
	bool fromString (const Vst::TChar* string, Vst::ParamValue& normValue) const SMTG_OVERRIDE;
};

//------------------------------------------------------------------------
// PanParameter Implementation
//------------------------------------------------------------------------
PanParameter::PanParameter (int32 flags, int32 id)
{
	Steinberg::UString (info.title, USTRINGSIZE (info.title)).assign (USTRING ("Pan"));
	Steinberg::UString (info.units, USTRINGSIZE (info.units)).assign (USTRING (""));

	info.flags = flags;
	info.id = id;
	info.stepCount = 0;
	info.defaultNormalizedValue = 0.5f;
	info.unitId = Vst::kRootUnitId;

	setNormalized (.5f);
}

//------------------------------------------------------------------------
void PanParameter::toString (Vst::ParamValue normValue, Vst::String128 string) const
{
	char text[32];
	if (normValue >= 0.505)
	{
		sprintf (text, "R %d", int32 ((normValue - 0.5f) * 200 + 0.5f));
	}
	else if (normValue <= 0.495)
	{
		sprintf (text, "L %d", int32 ((0.5f - normValue) * 200 + 0.5f));
	}
	else
	{
		strcpy (text, "C");
	}

	Steinberg::UString (string, 128).fromAscii (text);
}

//------------------------------------------------------------------------
bool PanParameter::fromString (const Vst::TChar* string, Vst::ParamValue& normValue) const
{
	String wrapper ((Vst::TChar*)string); // do not know buffer size here!
	if (wrapper.findFirst (STR ("C")) >= 0)
	{
		normValue = 0.5;
		return true;
	}
	else
	{
		bool left = wrapper.findFirst (STR ("L")) == 0;
		double tmp = 0.0;
		if (wrapper.scanFloat (tmp))
		{
			if (tmp < 0)
			{
				left = true;
				if (tmp < -100)
					tmp = 100;
				else
					tmp = -tmp;
			}
			else if (tmp > 100.0)
			{
				normValue = 1;
				return true;
			}
			if (!left)
				normValue = tmp / 200 + 0.5;
			else
				normValue = 0.5 - tmp / 200;

			return true;
		}
	}
	return false;
}

//-----------------------------------------------------------------------------
tresult PLUGIN_API PannerEditor::initialize (FUnknown* context)
{
	tresult result = EditController::initialize (context);
	if (result == kResultTrue)
	{
		//---Create Parameters------------
		parameters.addParameter (STR16 ("Bypass"), nullptr, 1, 0,
		                         Vst::ParameterInfo::kCanAutomate | Vst::ParameterInfo::kIsBypass,
		                         PannerParams::kBypassId);

		auto* panParam = new PanParameter (Vst::ParameterInfo::kCanAutomate, PannerParams::kParamPanId);
		parameters.addParameter (panParam);
	}
	return kResultTrue;
}

//------------------------------------------------------------------------
/*IPlugView* PLUGIN_API PannerEditor::createView(const char* _name)
{
	ConstString name (_name);
	if (name == Vst::ViewType::kEditor)
	{
		auto* view = new VST3Editor (this, "view", "plug.uidesc");
		return view;
	}
	return nullptr;
}*/

//------------------------------------------------------------------------
tresult PLUGIN_API PannerEditor::setComponentState (IBStream* state)
{
	// we receive the current state of the component (processor part)
	// we read our parameters and bypass value...
	if (!state)
		return kResultFalse;

	IBStreamer streamer (state, kLittleEndian);

	float savedParam1 = 0.f;
	if (streamer.readFloat (savedParam1) == false)
		return kResultFalse;
	setParamNormalized (PannerParams::kParamPanId, savedParam1);

	// read the bypass
	int32 bypassState;
	if (streamer.readInt32 (bypassState) == false)
		return kResultFalse;
	setParamNormalized (kBypassId, bypassState ? 1 : 0);

	return kResultOk;
}

//------------------------------------------------------------------------
} // namespace
} // namespace Steinberg
