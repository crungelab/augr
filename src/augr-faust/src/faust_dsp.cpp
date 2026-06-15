#include <fmt/core.h>

#include <augr/core/audio.h>
#include <augr/faust/faust_dsp.h>
#include <augr/faust/faust_dsp_ui.h>
#include <augr/rack/audio_pin.h>

namespace augr {

void FaustDsp::OnCreate() {
    Dsp::OnCreate();
    init(Audio::sample_rate());
}

void FaustDsp::CreateControls() {
    FaustDspUi ui(*this);
    buildUserInterface(&ui);
}

void FaustDsp::CreatePins() {
    int nInputs = getNumInputs();
    if (nInputs > 0) {
        ChannelLayout inputLayout = Audio::ChannelCountToLayout(nInputs);
        audio_in_ = new AudioInput(*this, "audio_in", inputLayout);
        AddInput(*audio_in_);
    }
    //
    int nOutputs = getNumOutputs();
    if (nOutputs > 0) {
        ChannelLayout outputLayout = Audio::ChannelCountToLayout(nOutputs);
        audio_out_ = new AudioOutput(*this, "audio_out", outputLayout);
        AddOutput(*audio_out_);
    }
}

void FaustDsp::Process() {
    control();

    // Source node: no input pin, always generate.
    if (!audio_in_) {
        Audio output(audio_out_->layout_);
        compute(output.frames(), nullptr, output.buffers());
        audio_out_->Write(output);
        return;
    }

    // Effect node: pass through silence if input is unconnected.
    auto input = audio_in_->Read();
    if (input.layout() == ChannelLayout::kNull) {
        audio_out_->Write(input);
        return;
    }

    // Effect node with live input: run DSP.
    Audio output(audio_out_->layout_);
    compute(output.frames(), input.buffers(), output.buffers());
    audio_out_->Write(output);
}

} // namespace augr