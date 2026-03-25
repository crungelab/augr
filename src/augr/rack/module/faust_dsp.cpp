#include <fmt/core.h>

#include <augr/rack/audio_pin.h>
#include <augr/rack/module/faust_dsp.h>
#include <augr/rack/module/faust_dsp_ui.h>
#include <augr/audio.h>

namespace augr {

bool FaustDsp::Create(Part &owner) {
    Dsp::Create(owner);
    init(Audio::sampleRate());
    FaustDspUi &ui = *new FaustDspUi(*this);
    buildUserInterface(&ui);
    //
    int nInputs = getNumInputs();
    if (nInputs > 0) {
        Audio::Format inputFormat = Audio::ChannelCountToFormat(nInputs);
        audio_in_ = new AudioPin(*this, "audio_in_", inputFormat);
        AddInput(*audio_in_);
    }
    //
    int nOutputs = getNumOutputs();
    if (nOutputs > 0) {
        Audio::Format outputFormat = Audio::ChannelCountToFormat(nOutputs);
        audio_out_ = new AudioPin(*this, "audio_out_", outputFormat);
        AddOutput(*audio_out_);
    }

    return true;
}

/*Audio FaustDsp::ProcessAudio(Audio input)
{
  Audio output(Audio::Format::kMono);
  compute(output.frames(), nullptr, output.buffers());
  return output;
}*/

Audio FaustDsp::ProcessAudio(Audio input) {
    /*Audio::Format format = Audio::Format::kMono;
    auto numInputs = getNumInputs();
    auto numOutputs = getNumOutputs();
    switch (numOutputs)
    {
    case 2:
      format = Audio::Format::kStereo;
    }*/
    Audio::Format format = audio_out_->format_;
    Audio output(format);

    if (audio_in_) {
        compute(output.frames(), input.buffers(), output.buffers());
    } else {
        compute(output.frames(), nullptr, output.buffers());
    }
    return output;
}

void FaustDsp::Process() {
    control();
    if (audio_in_) {
        auto input = audio_in_->Read();
        if (input.format_ != Audio::Format::kNull) {
            audio_out_->Write(ProcessAudio(input));
        }
    } else {
        audio_out_->Write(ProcessAudio());
    }
}

} // namespace augr