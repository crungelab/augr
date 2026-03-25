#include <iostream>

#include "plug.h"
#include "parameters.h"

#include "public.sdk/source/vst/vstaudioprocessoralgo.h"
#include "base/source/fstreamer.h"
#include "pluginterfaces/base/ibstream.h"
#include "pluginterfaces/vst/ivstparameterchanges.h"

namespace Steinberg {
namespace Panner {

#ifndef kPI
#define kPI 3.14159265358979323846
#endif

#define kRampingTimeMs 10.0 // in ms

//-----------------------------------------------------------------------------
Plug::Plug ()
{
	// register its editor class
	setControllerClass (MyControllerUID);

	// default init
	processAudioPtr = &Plug::processAudio<float>;
}

//-----------------------------------------------------------------------------
tresult PLUGIN_API Plug::initialize (FUnknown* context)
{
	//---always initialize the parent-------
	tresult result = AudioEffect::initialize (context);
	if (result != kResultTrue)
		return kResultFalse;

	//---create Audio In/Out busses------
	// we want a Mono Input and a Stereo Output
	addAudioInput (STR16 ("AudioInput"), Vst::SpeakerArr::kMono);
	addAudioOutput (STR16 ("AudioOutput"), Vst::SpeakerArr::kStereo);

	return kResultTrue;
}

//------------------------------------------------------------------------
tresult PLUGIN_API Plug::canProcessSampleSize (int32 symbolicSampleSize)
{
	return ((symbolicSampleSize == Vst::kSample32) || (symbolicSampleSize == Vst::kSample64)) ?
	           kResultTrue :
	           kResultFalse;
}

//-----------------------------------------------------------------------------
tresult PLUGIN_API Plug::setBusArrangements (Vst::SpeakerArrangement* inputs, int32 numIns,
                                                      Vst::SpeakerArrangement* outputs,
                                                      int32 numOuts)
{
	// we only support mono to stereo
	if (numIns == 1 && numOuts == 1 && inputs[0] == Vst::SpeakerArr::kMono &&
	    outputs[0] == Vst::SpeakerArr::kStereo)
	{
		return AudioEffect::setBusArrangements (inputs, numIns, outputs, numOuts);
	}
	return kResultFalse;
}

//-----------------------------------------------------------------------------
tresult PLUGIN_API Plug::setupProcessing (Vst::ProcessSetup& setup)
{
	if (setup.symbolicSampleSize == Vst::kSample64)
		processAudioPtr = &Plug::processAudio<double>;
	else
		processAudioPtr = &Plug::processAudio<float>;

	return AudioEffect::setupProcessing (setup);
}

//-----------------------------------------------------------------------------
tresult PLUGIN_API Plug::setActive (TBool state)
{
	return AudioEffect::setActive (state);
}

//-----------------------------------------------------------------------------
tresult PLUGIN_API Plug::process (Vst::ProcessData& data)
{
	//--- Read inputs parameter changes-----------
	if (data.inputParameterChanges)
	{
		int32 numParamsChanged = data.inputParameterChanges->getParameterCount ();
		for (int32 index = 0; index < numParamsChanged; index++)
		{
			if (Vst::IParamValueQueue* paramQueue =
			        data.inputParameterChanges->getParameterData (index))
			{
				Vst::ParamValue value;
				int32 sampleOffset;
				int32 numPoints = paramQueue->getPointCount ();
				switch (paramQueue->getParameterId ())
				{
					case PannerParams::kParamPanId:
						if (paramQueue->getPoint (numPoints - 1, sampleOffset, value) ==
						    kResultTrue)
							mPanValue = value;
						break;

					case PannerParams::kBypassId:
						if (paramQueue->getPoint (numPoints - 1, sampleOffset, value) ==
						    kResultTrue)
							std::cout << "PannerProcessor::Bypass = " << value << std::endl << std::flush;
							mBypass = (value > 0.5f);
						break;
				}
			}
		}
	}

	//--- Process Audio---------------------
	//--- ----------------------------------
	if (data.numInputs == 0 || data.numOutputs == 0 || data.numSamples == 0)
	{
		// nothing to do
		return kResultOk;
	}

	return (this->*processAudioPtr) (data);
}

//------------------------------------------------------------------------
template <typename SampleType>
tresult Plug::processAudio (Vst::ProcessData& data)
{
	int32 numFrames = data.numSamples;

	uint32 sampleFramesSize = getSampleFramesSizeInBytes (processSetup, numFrames);
	auto** currentInputBuffers =
	    (SampleType**)Vst::getChannelBuffersPointer (processSetup, data.inputs[0]);
	auto** currentOutputBuffers =
	    (SampleType**)Vst::getChannelBuffersPointer (processSetup, data.outputs[0]);

	// if we have only silence clear the output and do nothing.
	data.outputs->silenceFlags = data.inputs->silenceFlags ? 0x7FFFF : 0;
	if (data.inputs->silenceFlags)
	{
		memset (currentOutputBuffers[0], 0, sampleFramesSize);
		memset (currentOutputBuffers[1], 0, sampleFramesSize);

		return kResultOk;
	}

	float leftPan;
	float rightPan;
	if (mBypass)
		getStereoPanCoef (kPanLawEqualPower, 0.f, leftPan, rightPan);
	else
		getStereoPanCoef (kPanLawEqualPower, mPanValue, leftPan, rightPan);

	//---pan : 1 -> 2---------------------
	SampleType tmp;
	SampleType* inputMono = currentInputBuffers[0];
	SampleType* outputLeft = currentOutputBuffers[0];
	SampleType* outputRight = currentOutputBuffers[1];

	for (int32 n = 0; n < numFrames; n++)
	{
		tmp = inputMono[n];
		outputLeft[n] = tmp * leftPan;
		outputRight[n] = tmp * rightPan;
	}

	return kResultOk;
}

//------------------------------------------------------------------------
void Plug::getStereoPanCoef (int32 panType, float pan, float& left, float& right) const
{
	if (panType == kPanLawEqualPower)
	{
		pan = pan * kPI * 0.5f;
		left = cosf (pan);
		right = sinf (pan);
	}
	else
	{
		left = 0.5f;
		right = 0.5f;
	}
}

//------------------------------------------------------------------------
tresult PLUGIN_API Plug::setState (IBStream* state)
{
	if (!state)
		return kResultFalse;

	// called when we load a preset or project, the model has to be reloaded

	IBStreamer streamer (state, kLittleEndian);

	float savedPan= 0.f;
	if (streamer.readFloat (savedPan) == false)
		return kResultFalse;

	int32 savedBypass = 0;
	if (streamer.readInt32 (savedBypass) == false)
		return kResultFalse;

	mPanValue = savedPan;
	mBypass = savedBypass > 0;

	return kResultOk;
}

//------------------------------------------------------------------------
tresult PLUGIN_API Plug::getState (IBStream* state)
{
	// here we need to save the model (preset or project)

	float toSavePan = mPanValue;
	int32 toSaveBypass = mBypass ? 1 : 0;

	IBStreamer streamer (state, kLittleEndian);
	streamer.writeFloat (toSavePan);
	streamer.writeInt32 (toSaveBypass);

	return kResultOk;
}

//------------------------------------------------------------------------
} // namespace
} // namespace Steinberg
