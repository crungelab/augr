#include <fmt/core.h>

#include <augr/core/rack/audio_pin.h>
#include <augr/core/rack/module/faust_dsp.h>
#include <augr/core/rack/module/faust_dsp_ui.h>
#include <augr/core/audio.h>

namespace augr {

bool FaustDsp::Create(Part &owner) {
    Dsp::Create(owner);
    init(Audio::sampleRate());
    FaustDspUi &ui = *new FaustDspUi(*this);
    buildUserInterface(&ui);
    //
    int nInputs = getNumInputs();
    if (nInputs > 0) {
        ChannelLayout inputLayout = Audio::ChannelCountToLayout(nInputs);
        audio_in_ = new AudioPin(*this, "audio_in_", inputLayout);
        AddInput(*audio_in_);
    }
    //
    int nOutputs = getNumOutputs();
    if (nOutputs > 0) {
        ChannelLayout outputLayout = Audio::ChannelCountToLayout(nOutputs);
        audio_out_ = new AudioPin(*this, "audio_out_", outputLayout);
        AddOutput(*audio_out_);
    }

    return true;
}

Audio FaustDsp::ProcessAudio(Audio input) {
    ChannelLayout layout = audio_out_->layout_;
    Audio output(layout);

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
        if (input.layout_ != ChannelLayout::kNull) {
            audio_out_->Write(ProcessAudio(input));
        }
    } else {
        audio_out_->Write(ProcessAudio());
    }
}

} // namespace augr