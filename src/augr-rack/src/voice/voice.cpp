#include <augr/core/model_factory.h>

#include <augr/rack/module/audio_io.h>
#include <augr/rack/module/cv_io.h>
#include <augr/rack/module/midi_io.h>

#include <augr/rack/voice/voice.h>
#include <augr/rack/voice/voice_manager.h>

#include <augr/rack/rack.h>

namespace augr {

void Voice::OnDestroy() {
    if (create_mode_ != CreateMode::Replicated)
        rack().voice_manager().RemoveVoice(label_);
}

void Voice::OnCreate() {
    Subrack::OnCreate();
    label_ = "Voice";
}

void Voice::OnCreateFresh() {
    Subrack::OnCreateFresh();

    label_ = rack().voice_manager().AllocateUniqueName("Voice");
    rack().voice_manager().AddVoice(label_, this);

    auto midi_in_module = Model::Make<MidiInputModule>(shared_from_this());
    midi_in_module_ = midi_in_module.get();
    midi_in_ = midi_in_module_->midi_in_;

    auto audio_out_module = Model::Make<AudioOutputModule>(shared_from_this());
    audio_out_module_ = audio_out_module.get();
    audio_out_ = audio_out_module_->audio_out_;

    auto done_out_module = Model::Make<CvOutputModule>(shared_from_this());
    done_out_module_ = done_out_module.get();
    done_out_ = done_out_module_->cv_out_;
}

void Voice::OnCreateLoaded() { 
    Subrack::OnCreateLoaded();
    rack().voice_manager().AddVoice(label_, this); 
}

void Voice::OnCreateCopied() {
    Subrack::OnCreateCopied();
    label_ = rack().voice_manager().AllocateUniqueName(label_);
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
        return false;
    constexpr fy_real kSilence = 1e-3f;
    const auto &arr = env.array();
    return arr[arr.size() - 1] > kSilence;
}

void Voice::DeliverMidi(const MidiMessage &msg) {
    if (midi_in_module_)
        midi_in_module_->midi_out_->Write(msg);
}

void Voice::LinkTo(Voice& master) const {
    for (auto& module : modules_) {
        const Module * master_module = dynamic_cast<Module*>(master.FindByUuid(module->uuid()));
        if (!master_module) continue;
        for (auto& param : module->parameters()) {
            auto* master_param = master_module->FindParameter(param->label());
            if (!master_param) continue;
            param->LinkTo(*master_param);
        }
    }
}

} // namespace augr

using namespace augr;
DEFINE_MODEL_FACTORY(Voice, "Voice", "General")