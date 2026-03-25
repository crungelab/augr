#pragma once

#include "public.sdk/source/vst/vstaudioeffect.h"

namespace Steinberg {
namespace Panner {

//-----------------------------------------------------------------------------
class Plug : public Vst::AudioEffect
{
public:
	Plug ();

	tresult PLUGIN_API initialize (FUnknown* context) SMTG_OVERRIDE;
	tresult PLUGIN_API setBusArrangements (Vst::SpeakerArrangement* inputs, int32 numIns,
	                                       Vst::SpeakerArrangement* outputs,
	                                       int32 numOuts) SMTG_OVERRIDE;

	tresult PLUGIN_API canProcessSampleSize (int32 symbolicSampleSize) SMTG_OVERRIDE;
	tresult PLUGIN_API setupProcessing (Vst::ProcessSetup& setup) SMTG_OVERRIDE;
	tresult PLUGIN_API setActive (TBool state) SMTG_OVERRIDE;
	tresult PLUGIN_API process (Vst::ProcessData& data) SMTG_OVERRIDE;

//------------------------------------------------------------------------
	tresult PLUGIN_API setState (IBStream* state) SMTG_OVERRIDE;
	tresult PLUGIN_API getState (IBStream* state) SMTG_OVERRIDE;

	static FUnknown* createInstance (void*) { return (Vst::IAudioProcessor*)new Plug (); }

	enum
	{
		kPanLawEqualPower = 0,
	};

protected:
	void getStereoPanCoef (int32 panType, float pan, float& left, float& right) const;
	
	template <typename SampleType>
	tresult processAudio (Vst::ProcessData& data);
	tresult (Plug::*processAudioPtr) (Vst::ProcessData& data);
		
	Vst::ParamValue mPanValue = 0;

	bool mBypass = false;
};

//------------------------------------------------------------------------
} // namespace Panner
} // namespace Steinberg
