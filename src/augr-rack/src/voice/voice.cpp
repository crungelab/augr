//#include <augr/core/model_factory.h>

#include <augr/rack/module/midi_io.h>
#include <augr/rack/module/audio_io.h>
#include <augr/rack/module/cv_io.h>

#include <augr/rack/voice/voice.h>
#include <augr/rack/voice/voice_factory.h>

namespace augr {

/*
void Voice::Create(Model *parent) {
    Subrack::Create(parent);

    midi_in_module_ = new MidiInputModule();
    midi_in_module_->Create(this);
    midi_in_ = midi_in_module_->midi_in_;

    audio_out_module_ = new AudioOutputModule();
    audio_out_module_->Create(this);
    audio_out_ = audio_out_module_->audio_out_;

    done_out_module_ = new CvOutputModule();
    done_out_module_->Create(this);
    done_out_ = done_out_module_->cv_out_;
}
*/

void Voice::CreateDefaultIo() {
    midi_in_module_ = new MidiInputModule();
    midi_in_module_->Create(this);
    midi_in_ = midi_in_module_->midi_in_;

    audio_out_module_ = new AudioOutputModule();
    audio_out_module_->Create(this);
    audio_out_ = audio_out_module_->audio_out_;

    done_out_module_ = new CvOutputModule();
    done_out_module_->Create(this);
    done_out_ = done_out_module_->cv_out_;
}

void Voice::OnAddingChild(Model &model) {
    Subrack::OnAddingChild(model);

    if (auto *d = dynamic_cast<Io *>(&model)) {
        OnAddingIo(*d);
    }
}

void Voice::OnRemovingChild(Model &model) {
    if (auto *d = dynamic_cast<Io *>(&model)) {
        OnRemovingIo(*d);
    }
    Subrack::OnRemovingChild(model);
}

void Voice::OnAddingIo(Io &io) {
    if (auto *audio_output = dynamic_cast<AudioOutputModule *>(&io)) {
        audio_out_module_ = audio_output;
        audio_out_ = audio_out_module_->audio_out_;
    } else if (auto *midi_input = dynamic_cast<MidiInputModule *>(&io)) {
        midi_in_module_ = midi_input;
        midi_in_ = midi_in_module_->midi_in_;
    }
}

void Voice::OnRemovingIo(Io &io) {
    if (audio_out_module_ == &io)
        audio_out_module_ = nullptr;
    if (midi_in_module_ == &io)
        midi_in_module_ = nullptr;
}

bool Voice::IsActive() const {
    // High = done, low = still active.
    //return done_out_->Value() < 0.5f;
    auto voltage = done_out_->Read();
    //auto value = voltage.array()[0];
    fy_real value = 0;
    if (!voltage.Empty()) {
        value = voltage.array()[0];
    }
    return value < 0.5f;
}

} // namespace augr

using namespace augr;
//DEFINE_MODEL_FACTORY(Voice, "Voice", "General")
DEFINE_VOICE_FACTORY("Voice", "General")
