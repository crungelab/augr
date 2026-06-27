#include <augr/rack/pin/limiting_audio_input.h>

namespace augr {

Audio LimitingAudioInput::Reduce() const {
    if (slots_.empty())
        return Audio();

    Audio mixed(layout_);
    mixed.array().fill(0);

    for (auto *slot : slots_) {
        const auto slot_data = slot->Read();
        if (slot_data.Empty())
            continue;
        mixed.array() += slot_data.array();
    }

    limiter_.Process(mixed);
    return mixed;
}

} // namespace augr