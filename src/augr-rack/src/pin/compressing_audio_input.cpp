#include <augr/rack/pin/compressing_audio_input.h>

namespace augr {

Audio CompressingAudioInput::Reduce() const {
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

    compressor_.Process(mixed);   // mutable Compressor compressor_;
    return mixed;
}

} // namespace augr