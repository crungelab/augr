#pragma once

#include <augr/core/model_factory.h>
#include <augr/rack/voice/voice.h>
#include <augr/rack/voice/voice_manager.h>

namespace augr {

class VoiceFactory : public ModelFactoryT<Voice> {
public:
    using ModelFactoryT<Voice>::ModelFactoryT;

    Model *Produce(Model *parent = nullptr) override {
        Voice *v = Make(parent);
        // Allocate a unique name and register.
        v->label_ = VoiceManager::singleton().AllocateUniqueName("Voice");
        VoiceManager::singleton().AddVoice(v->label_, v);
        return v;
    }
};

#define DEFINE_VOICE_FACTORY(NAME, CATEGORY)                                  \
    VoiceFactory VoiceFactoryInstance(NAME, CATEGORY);                        \
    ModelFactory *GetVoiceFactory() { return &VoiceFactoryInstance; }

} // namespace augr