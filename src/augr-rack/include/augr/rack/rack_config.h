namespace augr {

struct RackConfig {
    int sample_rate = 48000;
    int frames = 512;
    int audio_input_channels = 0;
    int audio_output_channels = 2;
    // bool enable_audio_input = false;
    // bool enable_audio_output = true;
    bool enable_midi_input = true;
    bool enable_midi_output = false;
};

} // namespace augr