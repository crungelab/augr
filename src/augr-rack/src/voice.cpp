#include <augr/core/model_factory.h>

#include <augr/rack/voice.h>
#include <augr/rack/module/midi_io.h>
#include <augr/rack/module/audio_io.h>
#include <augr/rack/module/cv_io.h>

namespace augr {

void Voice::Create(Model *parent) {
    Subrack::Create(parent);
    label_ = "Voice";

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

/*
bool Voice::IsActive() const {
    // High = done, low = still active.
    //return done_out_->Value() < 0.5f;
    auto voltage = done_out_->Read();
    //auto value = voltage.array()[0];
    fy_real value = 0;
    if (voltage.array().size() != 0) {
        value = voltage.array()[0];
    }
    return value < 0.5f;
}
*/

} // namespace augr

using namespace augr;
DEFINE_MODEL_FACTORY(Voice, "Voice", "General")
