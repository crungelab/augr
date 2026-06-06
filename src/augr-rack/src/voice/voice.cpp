#include <augr/core/model_factory.h>

#include <augr/rack/module/audio_io.h>
#include <augr/rack/module/cv_io.h>
#include <augr/rack/module/midi_io.h>

#include <augr/rack/voice/voice.h>
#include <augr/rack/voice/voice_manager.h>

#include <augr/rack/rack.h>

namespace augr {

void Voice::Create() {
    Subrack::Create();
    label_ = "Voice";
}

void Voice::OnFresh() {
    label_ = rack().voice_manager().AllocateUniqueName("Voice");
    rack().voice_manager().AddVoice(label_, this);

    midi_in_module_ = &Model::Make<MidiInputModule>(this);
    midi_in_ = midi_in_module_->midi_in_;

    audio_out_module_ = &Model::Make<AudioOutputModule>(this);
    audio_out_ = audio_out_module_->audio_out_;

    done_out_module_ = &Model::Make<CvOutputModule>(this);
    done_out_ = done_out_module_->cv_out_;
}

void Voice::OnLoaded() {
    rack().voice_manager().AddVoice(label_, this);
}

void Voice::OnAddingIo(Io &io) {
    Subrack::OnAddingIo(io);
    if (auto *audio_output = dynamic_cast<AudioOutputModule *>(&io)) {
        audio_out_module_ = audio_output;
        audio_out_ = audio_out_module_->audio_out_;
    } else if (auto *midi_input = dynamic_cast<MidiInputModule *>(&io)) {
        midi_in_module_ = midi_input;
        midi_in_ = midi_in_module_->midi_in_;
    } else if (auto *cv_output = dynamic_cast<CvOutputModule *>(&io)) {
        done_out_module_ = cv_output;
        done_out_ = done_out_module_->cv_out_;
    }
}

void Voice::OnRemovingIo(Io &io) {
    if (audio_out_module_ == &io)
        audio_out_module_ = nullptr;
    if (midi_in_module_ == &io)
        midi_in_module_ = nullptr;
    if (done_out_module_ == &io)
        done_out_module_ = nullptr;

    Subrack::OnRemovingIo(io);
}

bool Voice::IsActive() const {
    auto env = done_out_->Read();
    if (env.Empty())
        return false;                   // no envelope -> treat as free
    constexpr fy_real kSilence = 1e-3f; // ~-60 dB; tune to taste
    const auto &arr = env.array();
    return arr[arr.size() - 1] > kSilence; // last sample = most recent state
}

/*
bool Voice::IsActive() const {
    // Low = done, high = still active.
    auto voltage = done_out_->Read();

    fy_real value = 0;
    if (!voltage.Empty()) {
        value = voltage.array()[0];
    }
    return value > 0.5f;
}
*/

void Voice::DeliverMidi(const MidiMessage &msg) {
    if (midi_in_module_) {
        midi_in_module_->midi_out_->Write(msg);
    }
}

} // namespace augr

using namespace augr;
DEFINE_MODEL_FACTORY(Voice, "Voice", "General")
